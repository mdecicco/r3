#pragma once
#include <r3/common/Types.h>
#include <r3/models/Models.h>

namespace r3 {
    namespace model {
        struct RawSession {
            u32 id;
            Datetime startedOn;

            // Null value indicates a crash or unexpected termination
            Datetime endedOn;
        };

        class SessionModel : public ModelBase<RawSession, "tblSession", SessionModel> {
            public:
                PrimaryKey<"id", &RawSession::id> id;
                Datetime<"started_on", &RawSession::startedOn> startedOn;
                Datetime<"ended_on", &RawSession::endedOn> endedOn;
        };
    };
};