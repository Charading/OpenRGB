#include "FanController.h"

void FanController::RegisterUpdateCallback(FanUpdateCallback callback, void* receiver)
{
    update_callbacks.push_back(std::make_pair(callback, receiver));
}

void FanController::UnregisterUpdateCallback(FanUpdateCallback callback, void* receiver)
{
    for(std::vector<FanUpdateBlock>::iterator it = update_callbacks.begin(); it != update_callbacks.end(); ++it)
    {
        if((it->first == callback || callback == nullptr) && it->second == receiver)
        {
            update_callbacks.erase(it);
        }
    }
}

void FanController::ClearCallbacks()
{
    update_callbacks.clear();
}
