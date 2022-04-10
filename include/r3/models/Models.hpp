#pragma once
#include <r3/models/Models.h>
#include <r3/models/SceneStorage.hpp>
#include <r3/utils/Database.hpp>
#include <r3/utils/Singleton.hpp>

// todo:
// Move this somewhere better
template <typename T>
struct p2m_traits { };

template <typename T, typename P>
struct p2m_traits<P T::*> {
    using ClassType = T;
    using MemberType = P;
};

namespace r3 {
    namespace model {
        template <typename RawModel, FixedString tableName, typename F>
        FieldBase<RawModel, tableName, F>::FieldBase() : m_field(nullptr) {
            ModelBase<RawModel, tableName, F>::m_fields.push_back(this);
        }

        template <typename RawModel, FixedString tableName, typename F>
        void FieldBase<RawModel, tableName, F>::init(db::Model<RawModel>* m) {
            m_field = create(m);
        }

        template <typename RawModel, FixedString tableName, typename F>
        db::IField* FieldBase<RawModel, tableName, F>::getField() {
            return m_field;
        }


        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr>
        db::IField* fPrimaryKey<RawModel, F, tableName, fieldName, FieldPtr>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->PrimaryKeyOffset(fieldName, FieldPtr);
            } else {
                return m->PrimaryKey(fieldName, FieldPtr);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, auto ForeignField, bool nullable, bool unique, FixedString checkCond>
        db::IField* fForeignKey<RawModel, F, tableName, fieldName, FieldPtr, ForeignField, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            using fkTraits = p2m_traits<decltype(ForeignField)>;
            using ForeignModel = fkTraits::ClassType;
            using FieldTp = fkTraits::MemberType;

            FieldTp& pk = Singleton<ForeignModel>::Get()->*ForeignField;
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->ForeignKeyOffset(fieldName, pk.getField(), FieldPtr, nullable, unique, checkCond);
            } else {
                return m->ForeignKey(fieldName, pk.getField(), FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, bool nullable, bool unique, FixedString checkCond>
        db::IField* fInteger<RawModel, F, tableName, fieldName, FieldPtr, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->IntegerOffset(fieldName, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->Integer(fieldName, FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, bool nullable, bool unique, FixedString checkCond>
        db::IField* fBigInteger<RawModel, F, tableName, fieldName, FieldPtr, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->BigIntegerOffset(fieldName, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->BigInteger(fieldName, FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, bool nullable, bool unique, FixedString checkCond>
        db::IField* fFloat<RawModel, F, tableName, fieldName, FieldPtr, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->FloatOffset(fieldName, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->Float(fieldName, FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, bool nullable, bool unique, FixedString checkCond>
        db::IField* fDatetime<RawModel, F, tableName, fieldName, FieldPtr, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->DatetimeOffset(fieldName, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->Datetime(fieldName, FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, bool nullable, bool unique, FixedString checkCond>
        db::IField* fBoolean<RawModel, F, tableName, fieldName, FieldPtr, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->BooleanOffset(fieldName, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->Boolean(fieldName, FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, u32 maxLen, bool nullable, bool unique, FixedString checkCond>
        db::IField* fString<RawModel, F, tableName, fieldName, FieldPtr, maxLen, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->StringOffset(fieldName, maxLen, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->String(fieldName, maxLen, FieldPtr, nullable, unique, checkCond);
            }
        }

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, u32 size, bool nullable, bool unique, FixedString checkCond>
        db::IField* fBlob<RawModel, F, tableName, fieldName, FieldPtr, size, nullable, unique, checkCond>::create(db::Model<RawModel>* m) {
            if constexpr (std::is_integral_v<decltype(FieldPtr)>) {
                return m->BlobOffset(fieldName, size, FieldPtr, nullable, unique, checkCond);
            } else {
                return m->Blob(fieldName, size, FieldPtr, nullable, unique, checkCond);
            }
        }



        template <typename RawModel, FixedString tableName, typename F>
        AutoModelRegistration<RawModel, tableName, F>::AutoModelRegistration() {
            didRegister = true;
            Singleton<F>::Create();
            registerModel(Singleton<F>::Get());
        }

        extern u64 __nextCompBit;

        template <typename RawModel, FixedString tableName, typename F>
        std::vector<FieldBase<RawModel, tableName, F>*> ModelBase<RawModel, tableName, F>::m_fields = {};

        template <typename RawModel, FixedString tableName, typename F>
        AutoModelRegistration<RawModel, tableName, F> ModelBase<RawModel, tableName, F>::m_auto_register = AutoModelRegistration<RawModel, tableName, F>();

        template <typename RawModel, FixedString tableName, typename F>
        u64 ModelBase<RawModel, tableName, F>::m_compBitmask = 0;

        template <typename RawModel, FixedString tableName, typename F>
        ModelBase<RawModel, tableName, F>::ModelBase() : db::Model<RawModel>(tableName) {
            if constexpr (std::is_base_of_v<mComponentBase, RawModel>) {
                ModelBase<RawModel, tableName, F>::m_compBitmask = 1ULL << (__nextCompBit++);
            }
        }

        template <typename RawModel, FixedString tableName, typename F>
        db::Model<RawModel>* ModelBase<RawModel, tableName, F>::Get() {
            return (db::Model<RawModel>*)Singleton<F>::Get();
        }

        template <typename RawModel, FixedString tableName, typename F>
        template <typename M>
        static std::enable_if_t<std::is_same_v<M, RawModel> && (std::is_same_v<mEntity, M> || std::is_base_of_v<mComponentBase, M>), SceneStorage<M>*>
        ModelBase<RawModel, tableName, F>::Storage() {
            return Singleton<SceneStorage<M>>::Get();
        }

        template <typename RawModel, FixedString tableName, typename F>
        template <typename M>
        std::enable_if_t<std::is_same_v<M, RawModel> && std::is_base_of_v<mComponentBase, M>, u64>
        ModelBase<RawModel, tableName, F>::Bitmask() {
            return ModelBase<RawModel, tableName, F>::m_compBitmask;
        }

        template <typename RawModel, FixedString tableName, typename F>
        void ModelBase<RawModel, tableName, F>::initModel(db::Model<RawModel>* self) {
        }

        template <typename RawModel, FixedString tableName, typename F>
        bool ModelBase<RawModel, tableName, F>::init() {
            if (ModelBase<RawModel, tableName, F>::m_auto_register.didRegister) {
                db::Model<RawModel>* m = (db::Model<RawModel>*)Singleton<F>::Get();

                for (auto* f : ModelBase<RawModel, tableName, F>::m_fields) {
                    f->init(m);
                }

                initModel(m);
                m->finalize();

                if constexpr (std::is_same_v<RawModel, mEntity>) {
                    db::ForeignKeyField* fk = (db::ForeignKeyField*)((F*)this)->sceneId.getField();
                    Singleton<SceneStorage<RawModel>>::Create(m, fk, 0);
                } else if constexpr(is_entity_component<RawModel>::value) {
                    db::ForeignKeyField* entityFk = nullptr;
                    // entity id won't always be the first field that's registered on the model...
                    for (auto* _f : ModelBase<RawModel, tableName, F>::m_fields) {
                        db::IField* f = _f->getField();
                        if (f->getType() != db::FieldType::ForeignKey) continue;
                        db::ForeignKeyField* fk = (db::ForeignKeyField*)f;
                        if (r3::String("tblEntity") != fk->getReference()->getTableName()) continue;
                        // Just to be sure...
                        if (fk->getOffset() != offsetof(mComponentBase, entityId)) continue;
                        entityFk = fk;
                        break;
                    }

                    if (entityFk) {
                        Singleton<SceneStorage<RawModel>>::Create(m, entityFk, ModelBase<RawModel, tableName, F>::Bitmask());
                    } else {
                        // Error will be reported later safely from a cpp file...
                        return false;
                    }
                }

                return true;
            }

            return false;
        }

        template <typename RawModel, FixedString tableName, typename F>
        void ModelBase<RawModel, tableName, F>::destroy() {
            if constexpr (std::is_same_v<RawModel, mEntity> || is_entity_component<RawModel>::value) {
                Singleton<SceneStorage<RawModel>>::Destroy();
            }
            Singleton<F>::Destroy();
        }

        template <typename RawModel, FixedString tableName, typename F>
        void ModelBase<RawModel, tableName, F>::initializeDb(db::Database* db) {
            ((db::Model<RawModel>*)Singleton<F>::Get())->initializeDb(db);
        }

        template <typename RawModel, FixedString tableName, typename F>
        db::ModelBase* ModelBase<RawModel, tableName, F>::getModelBase() {
            return (db::ModelBase*)(db::Model<RawModel>*)Singleton<F>::Get();
        }

        template <typename RawModel, FixedString tableName, typename F>
        bool ModelBase<RawModel, tableName, F>::initializeSceneStorage(SceneId sceneId) {
            if constexpr (std::is_same_v<RawModel, mEntity> || is_entity_component<RawModel>::value) {
                return Storage()->setSceneId(sceneId);
            } else return true;
        }

        template <typename RawModel, FixedString tableName, typename F>
        void ModelBase<RawModel, tableName, F>::populateSceneStorage() {
            if constexpr (std::is_same_v<RawModel, mEntity> || is_entity_component<RawModel>::value) {
                Storage()->load();
            }
        }

        template <typename RawModel, FixedString tableName, typename F>
        void ModelBase<RawModel, tableName, F>::persistSceneStorage() {
            if constexpr (std::is_same_v<RawModel, mEntity> || is_entity_component<RawModel>::value) {
                Storage()->save();
            }
        }
    };
};