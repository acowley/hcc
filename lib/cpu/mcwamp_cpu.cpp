//===----------------------------------------------------------------------===//
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include <cstdlib>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

#include <amp_runtime.h>

extern "C" void PushArgImpl(void *ker, int idx, size_t sz, const void *v) {}

namespace Concurrency {

class CPUFallbackDevice final : public KalmarDevice
{
public:
    CPUFallbackDevice() : KalmarDevice() {}

    std::wstring get_path() const override { return L"fallback"; }
    std::wstring get_description() const override { return L"CPU Fallback"; }
    size_t get_mem() const override { return 0; }
    bool is_double() const override { return true; }
    bool is_lim_double() const override { return true; }
    bool is_unified() const override { return true; }
    bool is_emulated() const override { return true; }

    void* create(size_t count, struct rw_info* /* not used */) override {
        return aligned_alloc(0x1000, count);
    }
    void release(void *data) override { ::operator delete(data); }
    std::shared_ptr<KalmarQueue> createQueue() override {
        return std::shared_ptr<KalmarQueue>(new KalmarQueue(this));
    }
};

class CPUContext final : public KalmarContext
{
public:
    CPUContext() { Devices.push_back(new CPUFallbackDevice); }
};


static CPUContext ctx;

} // namespace Concurrency

extern "C" void *GetContextImpl() {
  return &Concurrency::ctx;
}
