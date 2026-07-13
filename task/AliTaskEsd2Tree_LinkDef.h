#ifdef __CLING__
#pragma link off all globals;
#pragma link off all classes;
#pragma link off all functions;

// clang-format off
#pragma link C++ class  AliTaskEsd2Tree+;
#pragma link C++ struct POD::Event+;
#pragma link C++ struct POD::MC::Event+;
#pragma link C++ struct POD::Track+;
#pragma link C++ class  std::vector<POD::Track>+;
#pragma link C++ struct POD::PreFoundLambda+;
#pragma link C++ class  std::vector<POD::PreFoundLambda>+;
#pragma link C++ struct POD::InjectedSexa+;
#pragma link C++ class  std::vector<POD::InjectedSexa>+;
#pragma link C++ struct POD::McParticle+;
#pragma link C++ class  std::vector<POD::McParticle>+;
// clang-format on
#endif
