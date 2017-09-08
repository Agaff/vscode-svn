#include "client.h"

namespace Svn
{
Util_Method(Client::Cat)
{
    auto resolver = Util_NewMaybe(Promise::Resolver);
    Util_Return(resolver->GetPromise());

    Util_RejectIf(args.Length() == 0, Util_Error(TypeError, "Argument \"path\" must be a string"));

    auto arg = args[0];
    Util_RejectIf(!arg->IsString(), Util_Error(TypeError, "Argument \"path\" must be a string"));
    auto path = make_shared<string>(to_string(arg));
    Util_RejectIf(Util::ContainsNull(*path), Util_Error(Error, "Argument \"path\" must be a string without null bytes"));

    Util_PreparePool();

    auto buffer = svn_stringbuf_create_empty(pool.get());
    auto _error = make_shared<svn_error_t *>();
    auto work = [buffer, pool, client, path, _error]() -> void {
        apr_hash_t *props;

        auto stream = svn_stream_from_stringbuf(buffer, pool.get());

        svn_opt_revision_t revision{svn_opt_revision_working};

        apr_pool_t *scratch_pool;
        apr_pool_create(&scratch_pool, pool.get());

        *_error = svn_client_cat3(&props,          // props
                                  stream,          // out
                                  path->c_str(),   // path_or_url
                                  &revision,       // peg_revision
                                  &revision,       // revision
                                  false,           // expand_keywords
                                  client->context, // ctx
                                  pool.get(),      // result_pool
                                  scratch_pool);   // scratch_pool
    };

    auto _resolver = Util_SharedPersistent(Promise::Resolver, resolver);
    // Capture `pool` in `after_work` because I still need `buffer`
    auto after_work = [isolate, _resolver, _error, buffer, pool]() -> void {
        auto context = isolate->GetCallingContext();
        HandleScope scope(isolate);

        auto resolver = _resolver->Get(isolate);

        auto error = *_error;
        Util_RejectIf(error != SVN_NO_ERROR, SvnError::New(isolate, context, error->apr_err, error->message));

        auto result = node::Buffer::New(isolate, buffer->data, buffer->len).ToLocalChecked();
        resolver->Resolve(context, result);
        return;
    };

    Util_RejectIf(Util::QueueWork(uv_default_loop(), work, after_work), Util_Error(Error, "Failed to start async work"));
}
Util_MethodEnd;
}
