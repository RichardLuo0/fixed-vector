import std;
import makeDotCpp;
import makeDotCpp.project;
import makeDotCpp.compiler;
import makeDotCpp.fileProvider;
import makeDotCpp.builder;

using namespace makeDotCpp;

int main(int argc, const char **argv) {
  Project::OptionParser op;
  op.parse(argc, argv);
  const auto projectDesc = ProjectDesc::create(op.getPackagesPath());

  Clang clang;
  clang.addOption("-march=native -std=c++20 -Wall");

  if (!op.contains("debug")) {
    clang.addOption("-O3");
  }

  LibBuilder libBuilder;
  libBuilder.setName("fixedVector")
      .setCompiler(clang)
      .addSrc("src/FixedVector.cppm")
      .addSrc("src/Iter.cppm");

  ExeBuilder testBuilder;
  testBuilder.setName("test").setCompiler(clang).addSrc("tests/main.cpp");

  for (auto &package : projectDesc.packages) {
    libBuilder.addDepend(package);
    testBuilder.addDepend(package);
  }

  testBuilder.addDepend(libBuilder.getExport());

  Project()
      .setName("fixedVector")
      .setBuild([&](const Context &ctx) {
        try {
          testBuilder.build(ctx).get();
          std::cout << "\033[0;32mDone\033[0m" << std::endl;
        } catch (const CompileError &e) {
          std::cout << "\033[0;31merror: " << e.what() << "\033[0m"
                    << std::endl;
          throw e;
        }
      })
      .setRelease([](const Context &) {})
      .to("build")
      .run(op);
  return 0;
}
