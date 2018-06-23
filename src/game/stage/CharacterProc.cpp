#include "CharacterProc.hpp"

#ifdef _MSC_VER
// Avoid the zillions implicit conversion warnings
#pragma warning(disable: 4244)
#endif

// TODO: Remove gotos!
#define EMPTY_LABEL_HACK  { constexpr int dumb = 1; LP3_LOG_VAR(dumb) }

namespace nnd3d { namespace game {

CharacterProcManager::CharacterProcManager()
:   procs()
{
}

CharacterProcManager::~CharacterProcManager() {
    clear();
}

CharacterProcManager::CharacterProcManager(CharacterProcManager && rhs)
:   procs(std::move(rhs.procs)) {
}

void CharacterProcManager::add_process(gsl::owner<CharacterProc *> process) {
    // TODO: in the future, use the names here, or something, but for
    //       now, use the indexes to figure out if it's a player
    if (process != nullptr) {
        procs.push_back(process);
    }
}

void CharacterProcManager::animate(std::int64_t ms) {
    for (auto & p : procs) {
        p->animate(ms);
    }
}

void CharacterProcManager::clear() {
    for (gsl::owner<CharacterProc *> p : procs) {
        delete p;
    }
    procs.clear();
}

void CharacterProcManager::update() {
    for (int index = 0; lp3::narrow<std::size_t>(index) < procs.size();
            ++ index) {
        if (!procs[index]->update()) {
            delete procs[index];
            procs.erase(procs.begin() + index);
            index -= 1;
        }
    }
}

CharacterProcInterop::CharacterProcInterop(CharacterProcManager & manager_arg)
:   manager(manager_arg)
{}

void CharacterProcInterop::spawn(gsl::owner<CharacterProc *> process) {
    manager.add_process(process);
}


}   }
