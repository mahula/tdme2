#include <tdme/tests/EngineTest.h>

extern void init_jvm();
extern java::lang::StringArray* make_args(int args, char** argv);

int main(int argc, char** argv)
{
    init_jvm();
    
    ::tdme::tests::EngineTest::main(argc, argv);
    
    return 0;
}
