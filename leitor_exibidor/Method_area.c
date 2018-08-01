/**
*@file Method_area.case
*@author Gustavo Costa 14/0142568
*@author Andrei Buslik 11/0024702
*@brief Implementação da área de métodos da JVM.
*/

#include "Method_area.h"
#include "Conjunto_de_Instrucoes.h"

u1 opcode;
void (*instrucao[0xCA])(Frame *frame);
Pilha_frames *global_pilha;


int inicializa(ClassFile *classe, Pilha_frames *pilha) {
  int result = 0;
  for(int i = 0; i<classe->methods_count; i++) {
    char *nome;
    int local_nome = classe->constant_pool[i].utf8_info.length; //Ver esse indicacao de nome!!!
    nome = getConstantUTF8CP(classe->constant_pool, local_nome);
    if (!strcmp("<clinit>", nome) || !strcmp("<init>", nome)) { //verifica se e comeco do metodo
      method_info *init = &classe->methods[i];
      ready (init, classe, &pilha);
      executarMetodo (init, classe, pilha);
      result = 1;
    }
  }
  return result;
}

method_info* acessoMethod(char *metodo, ClassFile *classe) {

  for (int i = 0; i < classe->methods_count; i++) {
    int local_nome = classe->constant_pool[i].utf8_info.length; //Ver esse indicacao de nome!!!
    u1 *nome;
    nome  = (u1*) getConstantUTF8CP(classe->constant_pool, local_nome);
    if(!strcmp(metodo, (char *) nome)){
      return &classe->methods[i];
    }
  }
  return NULL;
}

void ready(method_info *method, ClassFile *classe, Pilha_frames **pilha){
  	for(int i = 0; i < method->attributes_count; i++) {
	    if (method->attributes[i].tag == ATTRIBUTE_Code) {
			if (method->attributes_count > 0) {
				Frame *frame = init_frame(method, classe);
				empilha_frame(pilha, frame);
				return;
			}
			else{
				method->attributes_count++;
				method->attributes = (attribute_info *) malloc(sizeof(attribute_info));
				method->attributes[0].code.code_length = 0;
				Frame *frame = init_frame(method, classe);
				empilha_frame(pilha, frame);
				return;
			}
	    }
  	}
	printf("Erro metodo sem Attribute code.\n");
	return;
}

int executarMetodo(method_info *metodo, ClassFile *classe, Pilha_frames *pilha) {
    int execucao = 0;

    while(!execucao) {
        Frame *metodoAtual = desempilha_frame(&pilha);
        execucao = executarInstrucoes(metodo, metodoAtual, pilha); // TODO: Função que executa intruções
        if(execucao == 45) { // Definir o que é ERRO_INSTRUCAO
            printf("ERRO: Instrucao nao pode ser executada.\n");
            desaloca_frame(metodoAtual);
            return 45 ;
        }
        if(!execucao) {
            empilha_frame(&pilha, metodoAtual);
        }
        else {
            desaloca_frame(metodoAtual);
        }
    }
    return 0; // 0 indica que todas as instruções do método atual foram executadas sem erro.
}

int executarInstrucoes (method_info *metodo, Frame *frame, Pilha_frames *pilha){
	printf("Executando instrucoes metodo\n");
	while(frame->pc < frame->codigo->code.code_length) {
        opcode = frame->codigo->code.code[frame->pc];
		printf("%d: %x\n", frame->pc, opcode);
		global_pilha = pilha;
		instrucao[opcode](frame);
		frame->pc++;
    }
    return 1;
}

void inicializa_Conj_Instrucoes(){
	// Constants
	instrucao[0x00] = nop; 					// 0
	instrucao[0x01] = aconst_null; 			// 1
	instrucao[0x02] = iconst_m1;
	instrucao[0x03] = iconst_0;
	instrucao[0x04] = iconst_1;
	instrucao[0x05] = iconst_2;
	instrucao[0x06] = iconst_3;
	instrucao[0x07] = iconst_4;
	instrucao[0x08] = iconst_5;
	instrucao[0x09] = lconst_0;
	instrucao[0x0A] = lconst_1;
	instrucao[0x0B] = fconst_0;
	instrucao[0x0C] = fconst_1;
	instrucao[0x0D] = fconst_2;
	instrucao[0x0E] = dconst_0;
	instrucao[0x0F] = dconst_1;
	instrucao[0x10] = decodebipush;
	instrucao[0x11] = decodesipush;
	instrucao[0x12] = decodeldc;
	instrucao[0x13] = decodeldc_w;
	instrucao[0x14] = decodeldc2_w;

	// // Loads
	// instrucao[0x15] = iload;
	// instrucao[0x16] = lload;
	// instrucao[0x17] = fload;
	// instrucao[0x18] = dload;
	// instrucao[0x19] = aload;
	instrucao[0x1A] = iload_0;
	instrucao[0x1B] = iload_1;
	instrucao[0x1C] = iload_2;
	instrucao[0x1D] = iload_3;
	// instrucao[0x1E] = lload_0;
	// instrucao[0x1F] = lload_1;
	// instrucao[0x20] = lload_2;
	// instrucao[0x21] = lload_3;
	// instrucao[0x22] = fload_0;
	// instrucao[0x23] = fload_1;
	// instrucao[0x24] = fload_2;
	// instrucao[0x25] = fload_3;
	// instrucao[0x26] = dload_0;
	// instrucao[0x27] = dload_1;
	// instrucao[0x28] = dload_2;
	// instrucao[0x29] = dload_3;
	instrucao[0x2A] = aload_0; 				// 42
	instrucao[0x2B] = aload_1;
	instrucao[0x2C] = aload_2;
	instrucao[0x2D] = aload_3;
	// instrucao[0x2E] = iaload;
	// instrucao[0x2F] = laload;
	// instrucao[0x30] = faload;
	// instrucao[0x31] = daload;
	// instrucao[0x32] = aaload;
	// instrucao[0x33] = baload;
	// instrucao[0x34] = caload;
	// instrucao[0x35] = saload;

	// //Store
	// instrucao[0x36] = istore;
	// instrucao[0x37] = lstore;
	// instrucao[0x38] = fstore;
	// instrucao[0x39] = dstore;
	// instrucao[0x3A] = astore;
	instrucao[0x3B] = istore_0;
	instrucao[0x3C] = istore_1;
	instrucao[0x3D] = istore_2;
	instrucao[0x3E] = istore_3;
	// instrucao[0x3F] = lstore_0;
	// instrucao[0x40] = lstore_1;
	// instrucao[0x41] = lstore_2;
	// instrucao[0x42] = lstore_3;
	// instrucao[0x43] = fstore_0;
	// instrucao[0x44] = fstore_1;
	// instrucao[0x45] = fstore_2;
	// instrucao[0x46] = fstore_3;
	// instrucao[0x47] = dstore_0;
	// instrucao[0x48] = dstore_1;
	// instrucao[0x49] = dstore_2;
	// instrucao[0x4A] = dstore_3;
	// instrucao[0x4B] = astore_0;
	// instrucao[0x4C] = astore_1;
	// instrucao[0x4D] = astore_2;
	// instrucao[0x4E] = astore_3;
	// instrucao[0x4F] = iastore;
	// instrucao[0x50] = lastore;
	// instrucao[0x51] = fastore;
	// instrucao[0x52] = dastore;
	// instrucao[0x53] = aastore;
	// instrucao[0x54] = bastore;
	// instrucao[0x55] = castore;
	// instrucao[0x56] = sastore;
	//
	// // Stack
	// instrucao[0x57] = pop;
	// instrucao[0x58] = pop2;
	// instrucao[0x59] = dup;
	// instrucao[0x5A] = dup_x1;
	// instrucao[0x5B] = dup_x2;
	// instrucao[0x5C] = dup2;
	// instrucao[0x5D] = dup2_x1;
	// instrucao[0x5E] = dup2_x2;
	// instrucao[0x5F] = swap;
	//
	// // Logic_Arit
	instrucao[0x60] = iadd;
	instrucao[0x61] = ladd;
	instrucao[0x62] = fadd;
	instrucao[0x63] = dadd;
	instrucao[0x64] = isub;
	instrucao[0x65] = lsub;
	instrucao[0x66] = fsub;
	instrucao[0x67] = dsub;
	instrucao[0x68] = imul;
	instrucao[0x69] = lmul;
	instrucao[0x6A] = fmul;
	instrucao[0x6B] = dmul;
	instrucao[0x6C] = idiv;
	instrucao[0x6D] = l_div;
	instrucao[0x6E] = fdiv;
	instrucao[0x6F] = ddiv;
	instrucao[0x70] = irem;
	instrucao[0x71] = lrem;
	instrucao[0x72] = frem;
	instrucao[0x73] = drem;
	instrucao[0x74] = ineg;
	instrucao[0x75] = lneg;
	// instrucao[0x76] = fneg;
	// instrucao[0x77] = dneg;
	// instrucao[0x78] = ishl
	// instrucao[0x79] = lshl;
	// instrucao[0x7A] = ishr;
	// instrucao[0x7B] = lshr;
	// instrucao[0x7C] = iushr;
	// instrucao[0x7D] = lushr;
	// instrucao[0x7E] = iand;
	// instrucao[0x7F] = land;
	// instrucao[0x80] = ior;
	// instrucao[0x81] = lor;
	// instrucao[0x82] = ixor;
	// instrucao[0x83] = lxor;
	// instrucao[0x84] = decodeiinc;
	//
	// // Conversion
	// instrucao[0x85] = i2l;
	// instrucao[0x86] = i2f;
	// instrucao[0x87] = i2d;
	// instrucao[0x88] = l2i;
	// instrucao[0x89] = l2f;
	// instrucao[0x8A] = l2d;
	// instrucao[0x8B] = f2i;
	// instrucao[0x8C] = f2l;
	// instrucao[0x8D] = f2d;
	// instrucao[0x8E] = d2i;
	// instrucao[0x8F] = d2l;
	// instrucao[0x90] = d2f;
	// instrucao[0x91] = i2b;
	// instrucao[0x92] = i2c;
	// instrucao[0x93] = i2s;
	//
	// // Comparisions
	// instrucao[0x94] = lcmp;
	// instrucao[0x95] = fcmp;
	// instrucao[0x96] = fcmpg;
	// instrucao[0x97] = dcmpl;
	// instrucao[0x98] = dcmpg;
	instrucao[0x99] = decodeifeq;
	instrucao[0x9A] = decodeifne;
	instrucao[0x9B] = decodeiflt;
	instrucao[0x9C] = decodeifge;
	instrucao[0x9D] = decodeifgt;
	instrucao[0x9E] = decodeifle;
	instrucao[0x9F] = decodeif_icmpeq;
	instrucao[0xA0] = decodeif_icmpne;
	instrucao[0xA1] = decodeif_icmplt;
	instrucao[0xA2] = decodeif_icmpge;
	instrucao[0xA3] = decodeif_icmpgt;
	instrucao[0xA4] = decodeif_icmple;
	instrucao[0xA5] = decodeif_acmpeq;
	instrucao[0xA6] = decodeif_acmpne;

	// // Control
	// instrucao[0xA7] = decodegoto_;
	// instrucao[0xA8] = decodejsr;
	// instrucao[0xA9] = decoderet;
	// instrucao[0xAA] = tableswtich;
	// instrucao[0xAB] = lookupswitch;
	// instrucao[0xAC] = ireturn;
	// instrucao[0xAD] = lreturn;
	// instrucao[0xAE] = freturn;
	// instrucao[0xAF] = dreturn;
	// instrucao[0xB0] = areturn;
	instrucao[0xB1] = return_;				// 177

	// // References
	instrucao[0xB2] = decodegetstatic;
	// instrucao[0xB3] = putstatic;
	// instrucao[0xB4] = decodegetfield;
	// instrucao[0xB5] = decodegetputfield;
	instrucao[0xB6] = decodeinvokevirtual;
	instrucao[0xB7] = decodeinvokeSpecial; 		// 183
	// instrucao[0xB8] = decodeinvokestatic;
	// instrucao[0xB9] = decodeinvokeinterface;
	// instrucao[0xBA] = invokedynamic;
	// instrucao[0xBB] = decodenew_;
	// instrucao[0xBC] = decodenewarray;
	// instrucao[0xBD] = decodeanewarray;
	// instrucao[0xBE] = arraylength;
	// instrucao[0xBF] = athrow;
	// instrucao[0xC0] = decodecheckcast;
	// instrucao[0xC1] = decodeinstanceof;
	// instrucao[0xC2] = monitorenter;
	// instrucao[0xC3] = monitorexit;
	//
	// // Extended
	// instrucao[0xC4] = decodewide;
	// instrucao[0xC5] = decodemultianewarray;
	// instrucao[0xC6] = decodeifnull
	// instrucao[0xC7] = decodeifnonull;
	// instrucao[0xC8] = decodegoto_w;
	// instrucao[0xC9] = decodejsr_w;
}

void decodeifeq(Frame* frame) {
	if_eq(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeifne(Frame* frame) {
	if_ne(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeiflt(Frame* frame) {
	if_lt(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeifge(Frame* frame) {
	if_ge(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeifgt(Frame* frame) {
	if_gt(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeifle(Frame* frame) {
	if_le(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_icmpeq(Frame* frame) {
	if_icmpeq(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_icmpne(Frame* frame) {
	if_icmpne(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_icmplt(Frame* frame) {
	if_icmplt(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_icmpge(Frame* frame) {
	if_icmpge(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_icmpgt(Frame* frame) {
	if_icmpgt(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_icmple(Frame* frame) {
	if_icmple(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_acmpeq(Frame* frame) {
	if_acmpeq(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeif_acmpne(Frame* frame) {
	if_acmpne(
		frame,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}

void decodegetstatic(Frame* frame) {
	getstatic(
		frame,
		global_pilha,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}

void decodeinvokevirtual(Frame* frame) {
	invokevirtual(
		frame,
		global_pilha,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
void decodeinvokeSpecial(Frame* frame) {
	invokespecial(
		frame,
		global_pilha,
		frame->codigo->code.code[++frame->pc],
		frame->codigo->code.code[++frame->pc]
	);
}
