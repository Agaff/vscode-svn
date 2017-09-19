#include "client.hpp"

namespace Svn
{
Util_Method(Client::ChangelistRemove)
{
    auto resolver = Util_NewMaybe(Promise::Resolver);
    auto promise = resolver->GetPromise();
    Util_Return(promise);

    Util_RejectIf(args.Length() == 0, Util_Error(TypeError, "Argument \"path\" must be a string or an array of string"));

    Util_PreparePool();

    apr_array_header_t *path;
    Util_ToAprStringArray(args[0], path);

    auto work = [path, client, pool]() -> svn_error_t * {
        SVN_ERR(svn_client_remove_from_changelists(path,               // paths
                                                   svn_depth_infinity, // depth
                                                   nullptr,            // changelists
                                                   client->context,    // ctx
                                                   pool.get()));       // scratch_pool

        return nullptr;
    };

    auto _resolver = Util_SharedPersistent(Promise::Resolver, resolver);
    auto after_work = [isolate, _resolver](svn_error_t *error) -> void {
        HandleScope scope(isolate);
        auto context = isolate->GetCallingContext();

        auto resolver = _resolver->Get(isolate);
        Util_RejectIf(error != SVN_NO_ERROR, SvnError::New(isolate, context, error));

        resolver->Resolve(context, Util_Undefined);
    };

    RunAsync();
}
Util_MethodEnd;
}