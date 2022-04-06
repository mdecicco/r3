#include <r3/r3.h>
#include <catch2/catch_test_macros.hpp>

using namespace r3;

struct DummyObject {
    u32 id;
    String name;
    i32 a, b, c;
    f32 x, y, z;
};

typedef Singleton<db::Model<DummyObject>> SDummyModel;
db::Model<DummyObject>* DummyModel = nullptr;

TEST_CASE("Database", "[db]") {
    Engine::Create();
    Engine::Get()->init();

    SDummyModel::Create("tblDummy");
    DummyModel = SDummyModel::Get();
    DummyModel->PrimaryKey("id", &DummyObject::id);
    DummyModel->String("name", 16, &DummyObject::name, false);
    DummyModel->Integer("a", &DummyObject::a);
    DummyModel->Integer("b", &DummyObject::b);
    DummyModel->Integer("c", &DummyObject::c);
    DummyModel->Float("x", &DummyObject::x);
    DummyModel->Float("y", &DummyObject::y);
    DummyModel->Float("z", &DummyObject::z);
    DummyModel->finalize();
    DummyModel->initializeDb(Engine::Get()->getDb());
    
    {
        Array<DummyObject> objs = Engine::Get()->getDb()->select(DummyModel);
        objs.each([](const DummyObject& o) {
            printf("%d: %s, %d, %d, %d, %.2f, %.2f, %.2f\n", o.id, o.name.c_str(), o.a, o.b, o.c, o.x, o.y, o.z);
        });

        printf("-------------------\n");

        Array<DummyObject> newObjs;
        for (u32 i = 0;i < 10;i++) {
            newObjs.push({
                0,
                String::Format("Dummy %d", i + objs.size()),
                rand() % 100,
                rand() % 100,
                rand() % 100,
                f32(rand()) / f32(RAND_MAX),
                f32(rand()) / f32(RAND_MAX),
                f32(rand()) / f32(RAND_MAX)
            });
        }
        printf("Pre-insert:\n");
        newObjs.each([](const DummyObject& o) {
            printf("%d: %s, %d, %d, %d, %.2f, %.2f, %.2f\n", o.id, o.name.c_str(), o.a, o.b, o.c, o.x, o.y, o.z);
        });
        printf("-------------------\n");

        Engine::Get()->getDb()->insertMany(DummyModel, newObjs, true);

        printf("Post-insert:\n");
        newObjs.each([](const DummyObject& o) {
            printf("%d: %s, %d, %d, %d, %.2f, %.2f, %.2f\n", o.id, o.name.c_str(), o.a, o.b, o.c, o.x, o.y, o.z);
        });
        printf("-------------------\n");
    }

    i32 result = Engine::Get()->execute();

    SDummyModel::Destroy();
    Engine::Destroy();
}
