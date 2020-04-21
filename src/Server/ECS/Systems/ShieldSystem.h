#pragma once

#include "System.h"

namespace spac::server::system {
    class ShieldSystem : public spac::System {
        using System::System;

        void update() override;
    };
}


