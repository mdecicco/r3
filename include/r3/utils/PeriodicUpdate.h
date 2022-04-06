#pragma once
#include <r3/common/Types.h>
#include <r3/utils/Timer.h>
#include <r3/utils/List.h>

namespace r3 {
    class IPeriodicUpdate {
        public:
            IPeriodicUpdate();
            ~IPeriodicUpdate();

            // Inverse of setUpdateInterval
            void setUpdateFrequency(f32 frequency);
            // Inverse of getUpdateInterval
            f32 getUpdateFrequency() const;

            // Inverse of setUpdateFrequency
            void setUpdateInterval(f32 interval);
            // Inverse of getUpdateFrequency
            f32 getUpdateInterval() const;

            void enableUpdates();
            void disableUpdates();
            void maybeUpdate(f32 deltaTime);
            void addChild(IPeriodicUpdate* child);
            void removeChild(IPeriodicUpdate* child);
            u32 getChildCount() const;

        protected:
            virtual void onUpdatesEnabled();
            virtual void onUpdatesDisabled();
            virtual void onUpdate(f32 frameDelta, f32 updateDelta);

        private:
            f32 m_targetInterval;
            Timer m_updateTimer;
            IPeriodicUpdate* m_parent;
            BidirectionalList<IPeriodicUpdate*> m_children;
    };
};