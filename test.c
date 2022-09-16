
#include "SVF-FE/LLVMUtil.h"
#include "Graphs/SVFG.h"
#include "Graphs/ICFG.h"
#include "WPA/Andersen.h"
#include "SABER/LeakChecker.h"
#include "llvm/IR/CFG.h"
#include <fstream>
#include <sstream>

#include "Graphs/VFG.h"
#include "Graphs/SVFG.h"
#include "WPA/Andersen.h"
#include "SVF-FE/SVFIRBuilder.h"
#include "SVF-FE/ICFGBuilder.h"
#include "Util/Options.h"
#include "MemoryModel/PAGBuilderFromFile.h"
#include "SABER/SaberSVFGBuilder.h"

using namespace SVF;
using namespace llvm;
using namespace std;

static llvm::cl::opt<std::string> InputFilename(cl::Positional,
        llvm::cl::desc("<input bitcode>"), llvm::cl::init("-"));

int main(int argc, char ** argv) {
  SVFModule *svfModule;
  SVFG *svfg;
  ICFG *icfg;

  int arg_num = 0;
  char **arg_value = new char *[argc];
  std::vector<std::string> moduleNameVec;
  LLVMUtil::processArguments(argc, argv, arg_num, arg_value, moduleNameVec);
  cl::ParseCommandLineOptions(arg_num, arg_value,
                              "analyze the vinilla distance of bb\n");

	svfModule = LLVMModuleSet::getLLVMModuleSet()->buildSVFModule(moduleNameVec);
  svfModule->buildSymbolTableInfo();

  SVFIRBuilder builder_svfir;
  SVFIR *svfir = builder_svfir.build(svfModule);
  AndersenWaveDiff *ander = AndersenWaveDiff::createAndersenWaveDiff(svfir);
  
  icfg = svfir->getICFG();

  PTACallGraph* callgraph = ander->getPTACallGraph();
  builder_svfir.updateCallGraph(callgraph);
  // icfg->dump("icfg_initial");
  icfg->updateCallGraph(callgraph);


  SVFGBuilder svfBuilder;
  svfg = svfBuilder.buildFullSVFG(ander);

  svfg->updateCallGraph(ander);
  // svfg->dump("svfg_final");
}