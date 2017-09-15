#include "client.hpp"

#define SetKind(name)                                                                     \
    Util::SetReadOnly(isolate, context, Kind, #name, Util_New(Integer, svn_node_##name)); \
    Util::SetReadOnly(isolate, context, Kind, svn_node_##name, Util_String(#name))

#define SetStatusKind(name)                                                                          \
    Util::SetReadOnly(isolate, context, StatusKind, #name, Util_New(Integer, svn_wc_status_##name)); \
    Util::SetReadOnly(isolate, context, StatusKind, svn_wc_status_##name, Util_String(#name))

#define SetDepth(name)                                                                      \
    Util::SetReadOnly(isolate, context, Depth, #name, Util_New(Integer, svn_depth_##name)); \
    Util::SetReadOnly(isolate, context, Depth, svn_depth_##name, Util_String(#name))

namespace Svn
{
Persistent<Function> Client::constructor;

void Client::Init(Local<Object> exports, Isolate *isolate, Local<Context> context)
{
    auto ClientTemplate = Util_FunctionTemplate(New, 0);
    ClientTemplate->SetClassName(String::NewFromUtf8(isolate, "Client"));
    // This internal field is used for saving the pointer to a Client instance.
    // Client.wrap will set its pointer to the internal field
    // And ObjectWrap::Unwrap will read the internal field and cast it to Client.
    ClientTemplate->InstanceTemplate()->SetInternalFieldCount(1);

    auto prototype = ClientTemplate->PrototypeTemplate();
    SetPrototypeMethod(ClientTemplate, prototype, "add", Add, 1);
    SetPrototypeMethod(ClientTemplate, prototype, "cat", Cat, 1);
    SetPrototypeMethod(ClientTemplate, prototype, "changelistAdd", ChangelistAdd, 2);
    SetPrototypeMethod(ClientTemplate, prototype, "changelistRemove", ChangelistRemove, 1);
    SetPrototypeMethod(ClientTemplate, prototype, "checkout", Checkout, 2);
    SetPrototypeMethod(ClientTemplate, prototype, "commit", Commit, 2);
    SetPrototypeMethod(ClientTemplate, prototype, "info", Info, 2);
    SetPrototypeMethod(ClientTemplate, prototype, "status", Status, 2);
    SetPrototypeMethod(ClientTemplate, prototype, "revert", Revert, 1);
    SetPrototypeMethod(ClientTemplate, prototype, "update", Update, 1);

    auto Client = ClientTemplate->GetFunction();

    auto Kind = Object::New(isolate);
    SetKind(none);
    SetKind(file);
    SetKind(dir);
    SetKind(unknown);
    Util_SetReadOnly2(Client, Kind);

    auto StatusKind = Object::New(isolate);
    SetStatusKind(none);
    SetStatusKind(unversioned);
    SetStatusKind(normal);
    SetStatusKind(added);
    SetStatusKind(missing);
    SetStatusKind(deleted);
    SetStatusKind(replaced);
    SetStatusKind(modified);
    SetStatusKind(conflicted);
    SetStatusKind(ignored);
    SetStatusKind(obstructed);
    SetStatusKind(external);
    SetStatusKind(incomplete);
    Util_SetReadOnly2(Client, StatusKind);

    auto Depth = Object::New(isolate);
    SetDepth(unknown);
    SetDepth(exclude);
    SetDepth(empty);
    SetDepth(files);
    SetDepth(immediates);
    SetDepth(infinity);
    Util_SetReadOnly2(Client, Depth);

    constructor.Reset(isolate, Client);
    Util_SetReadOnly2(exports, Client);
}
}
