#pragma once
#include <r3/common/Types.h>
#include <r3/models/Models.h>

namespace r3 {
    namespace model {
        struct mSession {
            u32 id;
            Datetime startedOn;

            // Null value indicates a crash or unexpected termination
            Datetime endedOn;
        };

        class SessionModel : public ModelBase<mSession, "tblSession", SessionModel> {
            public:
                PrimaryKey<"id", &Instance::id> id;
                Datetime<"started_on", &Instance::startedOn> startedOn;
                Datetime<"ended_on", &Instance::endedOn> endedOn;
        };
    };
};