#pragma once
#include <r3/common/Types.h>
#include <r3/utils/Database.h>
#include <r3/utils/Datetime.h>
#include <vector>

namespace r3 {
    namespace model {
        // https://www.youtube.com/watch?v=fnWqcJJxDOA

        typedef u32 SceneId;
        struct mEntity;
        struct mComponentBase;

        // FixedString template struct thanks to 'user'
        // https://stackoverflow.com/a/59495681

        template <unsigned N>
        struct FixedString {
            char buf[N + 1]{};
            int length = N;

            constexpr FixedString(char const* string) {
                for (unsigned index = 0;index < N;++index) {
                    buf[index] = string[index];
                }
            }
            constexpr operator char const*() const { return buf; }
        };

        template<unsigned N>
        FixedString(char const (&)[N]) -> FixedString<N - 1>;

        template <typename RawModel, FixedString tableName, typename F>
        class ModelBase;

        template <typename RawModel, FixedString tableName, typename F>
        struct FieldBase {
            FieldBase();
            
            void init(db::Model<RawModel>* m);
            db::IField* getField();

            protected:
                virtual db::IField* create(db::Model<RawModel>* m) = 0;
                db::IField* m_field;
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr>
        struct fPrimaryKey : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, auto ForeignField, bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fForeignKey : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr,                    bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fInteger : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr,                    bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fBigInteger : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr,                    bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fFloat : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, u32 maxLen,        bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fString : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr,                    bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fDatetime : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr,                    bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fBoolean : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename RawModel, typename F, FixedString tableName, FixedString fieldName, auto FieldPtr, u32 size,          bool nullable = false, bool unique = false, FixedString checkCond = "">
        struct fBlob : FieldBase<RawModel, tableName, F> {
            virtual db::IField* create(db::Model<RawModel>* m);
        };

        template <typename T, FixedString tableName, typename F>
        struct AutoModelRegistration {
            // this is only necessary to (hopefully) ensure that the
            // compiler doesn't optimize out the unused static members
            // of this type
            bool didRegister;

            AutoModelRegistration();
        };

        class PlainModelBase {
            public:
                virtual bool init() = 0;
                virtual void destroy() = 0;
                virtual void initializeDb(db::Database* db) = 0;
                virtual bool initializeSceneStorage(SceneId sceneId) = 0;
                virtual void populateSceneStorage() = 0;
                virtual void persistSceneStorage() = 0;
                virtual db::ModelBase* getModelBase() = 0;
        };

        template <typename Cls>
        class SceneStorage;

        template <typename RawModel, FixedString tableName, typename F>
        class ModelBase : public PlainModelBase, public db::Model<RawModel> {
            public:
                using Instance = RawModel;

                // Please forgive me
                template                                              <FixedString fieldName, auto FieldPtr>
                struct PrimaryKey : fPrimaryKey<RawModel, F, tableName,            fieldName,      FieldPtr> {};
                template                                              <FixedString fieldName, auto FieldPtr, auto ForeignFieldPtr, bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct ForeignKey : fForeignKey<RawModel, F, tableName,            fieldName,      FieldPtr,      ForeignFieldPtr,      nullable,              unique,                     checkCond> {};
                template                                              <FixedString fieldName, auto FieldPtr,                       bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct Integer    : fInteger   <RawModel, F, tableName,            fieldName,      FieldPtr,                            nullable,              unique,                     checkCond> { };
                template                                              <FixedString fieldName, auto FieldPtr,                       bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct BigInteger : fBigInteger<RawModel, F, tableName,            fieldName,      FieldPtr,                            nullable,              unique,                     checkCond> { };
                template                                              <FixedString fieldName, auto FieldPtr,                       bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct Float      : fFloat     <RawModel, F, tableName,            fieldName,      FieldPtr,                            nullable,              unique,                     checkCond> { };
                template                                              <FixedString fieldName, auto FieldPtr, u32 maxLen,           bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct String     : fString    <RawModel, F, tableName,            fieldName,      FieldPtr,     maxLen,                nullable,              unique,                     checkCond> { };
                template                                              <FixedString fieldName, auto FieldPtr,                       bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct Datetime   : fDatetime  <RawModel, F, tableName,            fieldName,      FieldPtr,                            nullable,              unique,                     checkCond> { };
                template                                              <FixedString fieldName, auto FieldPtr,                       bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct Boolean    : fBoolean   <RawModel, F, tableName,            fieldName,      FieldPtr,                            nullable,              unique,                     checkCond> { };
                template                                              <FixedString fieldName, auto FieldPtr, u32 size,             bool nullable = false, bool unique = false, FixedString checkCond = "">
                struct Blob       : fBlob      <RawModel, F, tableName,            fieldName,      FieldPtr,     size,                  nullable,              unique,                     checkCond> { };

                ModelBase();

                static db::Model<RawModel>* Get();

                template<typename M = RawModel>
                static std::enable_if_t<std::is_same_v<M, RawModel> && (std::is_same_v<mEntity, M> || std::is_base_of_v<mComponentBase, M>), SceneStorage<M>*> Storage();

                virtual void initModel(db::Model<RawModel>* self);
                virtual void destroy();
                virtual void initializeDb(db::Database* db);
                virtual db::ModelBase* getModelBase();
                virtual bool initializeSceneStorage(SceneId sceneId);
                virtual void populateSceneStorage();
                virtual void persistSceneStorage();

            private:
                friend struct FieldBase<RawModel, tableName, F>;
                virtual bool init();
                static std::vector<FieldBase<RawModel, tableName, F>*> m_fields;
                static AutoModelRegistration<RawModel, tableName, F> m_auto_register;
        };

        void registerModel(PlainModelBase* model);
        
        bool initializeModels();
        void destroyModels();
        void initializeDatabase(db::Database* db);

        bool initializeSceneStorage(SceneId sceneId);
        void populateSceneStorage();
        void persistSceneStorage();
    };
};