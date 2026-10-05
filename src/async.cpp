#include "async.h"

#include <memory>
#include <mutex>
#include <unordered_map>

#include "context.h"
#include "runtime.h"

namespace async
{

namespace
{

std::mutex g_contexts_mutex;
std::unordered_map<handle_t, std::shared_ptr<Context>> g_contexts;

std::shared_ptr<Context> findContext(handle_t handle)
{
    std::lock_guard<std::mutex> lock(g_contexts_mutex);
    const auto it = g_contexts.find(handle);
    if (it == g_contexts.end())
    {
        return nullptr;
    }
    return it->second;
}

} // namespace

handle_t connect(std::size_t bulk)
{
    Runtime &runtime = Runtime::instance();
    auto context = std::make_shared<Context>(bulk, runtime);

    std::lock_guard<std::mutex> lock(g_contexts_mutex);
    g_contexts[context.get()] = context;
    return context.get();
}

void receive(handle_t handle, const char *data, std::size_t size)
{
    const auto context = findContext(handle);
    if (context == nullptr)
    {
        return;
    }
    context->feed(data, size);
}

void disconnect(handle_t handle)
{
    std::shared_ptr<Context> context;
    {
        std::lock_guard<std::mutex> lock(g_contexts_mutex);
        const auto it = g_contexts.find(handle);
        if (it == g_contexts.end())
        {
            return;
        }
        context = it->second;
        g_contexts.erase(it);
    }
    context->finish();
}

} // namespace async
