#include <nan.h>
#include "nBody.h"

using v8::FunctionTemplate;
using v8::Handle;
using v8::Object;
using v8::String;
using Nan::GetFunction;
using Nan::New;
using Nan::Set;

// Expose synchronous and asynchronous access to our
// nBody
NAN_MODULE_INIT(InitAll) {
  Set(target, New<String>("nBody").ToLocalChecked(),
    GetFunction(New<FunctionTemplate>(nBody)).ToLocalChecked());
}

NODE_MODULE(addon, InitAll)
