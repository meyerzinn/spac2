#pragma once

#include <System.h>

namespace spac::server::system {

    class LifetimeSystem : public spac::System {
    public:
        using System::System;

        void update() override;
    };

}


