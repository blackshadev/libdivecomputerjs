#include "bluetooth.h"
#include "../errors/DCError.h"
#include "../iostream.h"

Napi::FunctionReference BluetoothTransport::constructor;

void BluetoothTransport::Init(Napi::Env env, Napi::Object exports)
{
    Napi::Function func = DefineClass(
        env,
        "BluetoothTransport",
        {
            StaticMethod<&BluetoothTransport::iterate>("iterate"),
            InstanceAccessor<&BluetoothTransport::getName>("name"),
            InstanceMethod<&BluetoothTransport::toString>("toString"),
        });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("BluetoothTransport", func);
}

Napi::Value BluetoothTransport::iterate(const Napi::CallbackInfo &info)
{
    dc_status_t status;
    auto env = info.Env();

    if (info.Length() != 2 || info[0].IsExternal() || info[1].IsExternal())
    {
        throw Napi::TypeError::New(info.Env(), "Invalid arguments,  expected {context} and {descriptor} as arguments");
    }

    auto context = Napi::ObjectWrap<Context>::Unwrap(info[0].As<Napi::Object>());
    auto descriptor = Napi::ObjectWrap<Descriptor>::Unwrap(info[1].As<Napi::Object>());

    auto array = Napi::Array::New(env);

    dc_iterator_t *iterator;
    status = dc_bluetooth_iterator_new(&iterator, context->getNative(), descriptor->getNative());

    DCError::AssertSuccess(env, status);

    dc_bluetooth_device_t *device;
    int i = 0;
    while ((status = dc_iterator_next(iterator, &device)) != DC_STATUS_DONE)
    {
        DCError::AssertSuccess(env, status);

        array[i++] = constructor.New({
            Napi::External<dc_bluetooth_device_t>::New(env, device),
        });
    }

    dc_iterator_free(iterator);

    return array;
}

BluetoothTransport::BluetoothTransport(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<BluetoothTransport>(info)
{
    if (info.Length() != 1 || !info[0].IsExternal())
    {
        throw Napi::TypeError::New(info.Env(), "Cannot construct Descriptor from JS. Use BluetoothTransport.iterate() to iterate all available.");
    }

    this->device = info[0].As<Napi::External<dc_bluetooth_device_t>>().Data();
}

BluetoothTransport::~BluetoothTransport()
{
    if (device)
    {
        dc_bluetooth_device_free(device);
        device = NULL;
    }
}

Napi::Value BluetoothTransport::getName(const Napi::CallbackInfo &info)
{
    return Napi::String::New(info.Env(), dc_bluetooth_device_get_name(device));
}

Napi::Value BluetoothTransport::toString(const Napi::CallbackInfo &info)
{
    return getName(info);
}

Napi::Value BluetoothTransport::open(const Napi::CallbackInfo &info)
{

    if (info.Length() != 1 || info[0].IsObject())
    {
        throw Napi::TypeError::New(info.Env(), "Invalid argument,  expected {context}.");
    }

    auto context = Napi::ObjectWrap<Context>::Unwrap(info[0].As<Napi::Object>());

    dc_iostream_t *iostream;
    auto status = getNative(&iostream, context->getNative());
    DCError::AssertSuccess(info.Env(), status);

    return IOStream::constructor.New(
        {
            Napi::External<dc_iostream_t>::New(info.Env(), iostream),
        });
}

dc_status_t BluetoothTransport::getNative(dc_iostream_t **iostream, dc_context_t *ctx)
{
    auto address = dc_bluetooth_device_get_address(device);
    return dc_bluetooth_open(iostream, ctx, address, 0);
}