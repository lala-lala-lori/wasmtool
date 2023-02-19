#include <wasmedge/wasmedge.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void usage(){
	printf("usage: \n");
	char* usage = "./wet [command] [wasm path] [arguments]\n"
		" version   prints the used wasmedge version \n"
		" run       Execute wasm file with arguments \n";
	printf("%s\n", usage);
	exit(1);
}

int main(int Argc, const char* Argv[]) {

    if(Argc == 2 && !strcmp(Argv[1], "version")) {
		printf("WasmEdge version: %s\n", WasmEdge_VersionGet());
		return 0;
	}
	if(Argc <= 1 || strcmp(Argv[1], "run")) {
		usage();
	}
	if(Argc < 3){
		printf("wasm path not specified!!\n\n");
		usage();
	}


  WasmEdge_ConfigureContext *ConfCxt = WasmEdge_ConfigureCreate();
  WasmEdge_ConfigureAddHostRegistration(ConfCxt, WasmEdge_HostRegistration_Wasi);
  WasmEdge_VMContext *VMCxt = WasmEdge_VMCreate(ConfCxt, NULL);

	
	/* Convert to wasmEdge_Value object */
	


	WasmEdge_Result Res;

	/* Load WASM FILE */
	Res = WasmEdge_VMLoadWasmFromFile(VMCxt, Argv[2]);
	
	if (!WasmEdge_ResultOK(Res)) {
		printf("Loading phase failed: %s\n", WasmEdge_ResultGetMessage(Res));
		return 1;
    }

	/* validate the wasm module */
	Res = WasmEdge_VMValidate(VMCxt);
	
	if (!WasmEdge_ResultOK(Res)) {
		printf("Validation phase failed: %s\n", WasmEdge_ResultGetMessage(Res));
		return 1;
	}

	/* Instantiate the WASM module */
	Res = WasmEdge_VMInstantiate(VMCxt);

	if (!WasmEdge_ResultOK(Res)) {
		printf("Instantiation phase failed: %s\n",
			WasmEdge_ResultGetMessage(Res));
		return 1;
	}


	WasmEdge_String Names[1];
    const WasmEdge_FunctionTypeContext* FuncTypes[1];
    uint32_t FuncListSize = WasmEdge_VMGetFunctionList(VMCxt, Names, FuncTypes, 1);

    uint32_t ParamLength = WasmEdge_FunctionTypeGetParametersLength(FuncTypes[0]);
    uint32_t ReturnLength = WasmEdge_FunctionTypeGetReturnsLength(FuncTypes[0]);

	if(Argc-3 < ParamLength) {
		printf("Invalid Arguments!\n");
		return 1;
	}
   
	WasmEdge_Value Params[Argc-3];
	for(int i=0; i<ParamLength; i++){
		Params[i] = WasmEdge_ValueGenI32(atoi(Argv[i+3]));
	}
	WasmEdge_Value Returns[1];

	//  debug
	//	char buf[256];
	//  uint32_t Strlen = WasmEdge_StringCopy(Names[0], buf, 256);
	//  printf("%s\n", buf);

	/* Execute WASM function */
	Res = WasmEdge_VMExecute(VMCxt, Names[0], Params, ParamLength, Returns, ReturnLength);
  
	if (WasmEdge_ResultOK(Res)) {
		printf("Get the result: %d\n", WasmEdge_ValueGetI32(Returns[0]));
	} else {
		printf("Execution phase failed: %s\n", WasmEdge_ResultGetMessage(Res));
	}
	

	/* Resources Deallocations. */
    WasmEdge_VMDelete(VMCxt);
	WasmEdge_ConfigureDelete(ConfCxt);
	return 0;

}
