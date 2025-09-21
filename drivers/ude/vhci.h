/*
 * Copyright (c) 2022-2025 Vadym Hrynchyshyn <vadimgrn@gmail.com>
 */

#pragma once

#include "context.h"

namespace usbip
{

_Function_class_(EVT_WDF_DRIVER_DEVICE_ADD)
_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
PAGED NTSTATUS DeviceAdd(_In_ WDFDRIVER, _Inout_ WDFDEVICE_INIT *init);

} // namespace usbip


namespace usbip::vhci
{

_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
int claim_roothub_port(_In_ UDECXUSBDEVICE device);

_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
int reclaim_roothub_port(_In_ UDECXUSBDEVICE device);

_IRQL_requires_same_
_IRQL_requires_max_(DISPATCH_LEVEL)
wdf::ObjectRef get_device(_In_ WDFDEVICE vhci, _In_ int port);

enum class detach_call { async_wait, async_nowait, direct };

_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
PAGED void detach_all_devices(_In_ WDFDEVICE vhci, _In_ detach_call how);

struct imported_device;
enum class state;

_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
PAGED NTSTATUS fill(_Out_ imported_device &dev, _In_ const device_attributes &r, _In_ int port);

inline auto fill(_Out_ imported_device &dev, _In_ const device_ctx &ctx)
{
        return fill(dev, ctx.attributes(), ctx.port);
}

_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
PAGED void complete_read(_In_ WDFREQUEST request, _In_ WDFMEMORY evt);

_IRQL_requires_same_
_IRQL_requires_(PASSIVE_LEVEL)
PAGED void device_state_changed(_In_ WDFDEVICE vhci, _In_ const device_attributes &attr, _In_ int port, _In_ state state);

inline void device_state_changed(_In_ const device_ctx &dev, _In_ state state)
{
        device_state_changed(dev.vhci, dev.attributes(), dev.port, state);
}

} // namespace usbip::vhci
