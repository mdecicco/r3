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
                fPrimaryKey<RawSession, SessionModel, "tblSession", "id", &RawSession::id> id;
                fDatetime<RawSession, SessionModel, "tblSession", "started_on", &RawSession::startedOn> startedOn;
                fDatetime<RawSession, SessionModel, "tblSession", "ended_on", &RawSession::endedOn> endedOn;
        };
    };
};