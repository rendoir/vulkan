#include <stdint.h>

#if 0
/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Sample-Programs/Hologram/Hologram.push_constant.vert
// Module Version 10000
// Generated by (magic number): 80007
// Id's are bound by 107

                              Capability Shader
               1:             ExtInstImport  "GLSL.std.450"
                              MemoryModel Logical GLSL450
                              EntryPoint Vertex 4  "main" 38 60 82 95 102
                              Source ESSL 310
                              Name 4  "main"
                              Name 9  "world_light"
                              Name 12  "param_block"
                              MemberName 12(param_block) 0  "light_pos"
                              MemberName 12(param_block) 1  "light_color"
                              MemberName 12(param_block) 2  "model"
                              MemberName 12(param_block) 3  "view_projection"
                              MemberName 12(param_block) 4  "alpha"
                              Name 14  "params"
                              Name 34  "world_pos"
                              Name 38  "in_pos"
                              Name 49  "world_normal"
                              Name 60  "in_normal"
                              Name 63  "light_dir"
                              Name 68  "brightness"
                              Name 80  "gl_PerVertex"
                              MemberName 80(gl_PerVertex) 0  "gl_Position"
                              MemberName 80(gl_PerVertex) 1  "gl_PointSize"
                              Name 82  ""
                              Name 95  "color"
                              Name 102  "alpha"
                              MemberDecorate 12(param_block) 0 Offset 0
                              MemberDecorate 12(param_block) 1 Offset 16
                              MemberDecorate 12(param_block) 2 ColMajor
                              MemberDecorate 12(param_block) 2 Offset 32
                              MemberDecorate 12(param_block) 2 MatrixStride 16
                              MemberDecorate 12(param_block) 3 ColMajor
                              MemberDecorate 12(param_block) 3 Offset 96
                              MemberDecorate 12(param_block) 3 MatrixStride 16
                              MemberDecorate 12(param_block) 4 Offset 160
                              Decorate 12(param_block) Block
                              Decorate 38(in_pos) Location 0
                              Decorate 60(in_normal) Location 1
                              MemberDecorate 80(gl_PerVertex) 0 BuiltIn Position
                              MemberDecorate 80(gl_PerVertex) 1 BuiltIn PointSize
                              Decorate 80(gl_PerVertex) Block
                              Decorate 95(color) Location 0
                              Decorate 102(alpha) Location 1
               2:             TypeVoid
               3:             TypeFunction 2
               6:             TypeFloat 32
               7:             TypeVector 6(float) 3
               8:             TypePointer Function 7(fvec3)
              10:             TypeVector 6(float) 4
              11:             TypeMatrix 10(fvec4) 4
 12(param_block):             TypeStruct 7(fvec3) 7(fvec3) 11 11 6(float)
              13:             TypePointer PushConstant 12(param_block)
      14(params):     13(ptr) Variable PushConstant
              15:             TypeInt 32 1
              16:     15(int) Constant 2
              17:             TypePointer PushConstant 11
              20:     15(int) Constant 0
              21:             TypePointer PushConstant 7(fvec3)
              24:    6(float) Constant 1065353216
              37:             TypePointer Input 7(fvec3)
      38(in_pos):     37(ptr) Variable Input
              52:             TypeMatrix 7(fvec3) 3
   60(in_normal):     37(ptr) Variable Input
              67:             TypePointer Function 6(float)
80(gl_PerVertex):             TypeStruct 10(fvec4) 6(float)
              81:             TypePointer Output 80(gl_PerVertex)
              82:     81(ptr) Variable Output
              83:     15(int) Constant 3
              92:             TypePointer Output 10(fvec4)
              94:             TypePointer Output 7(fvec3)
       95(color):     94(ptr) Variable Output
              96:     15(int) Constant 1
             101:             TypePointer Output 6(float)
      102(alpha):    101(ptr) Variable Output
             103:     15(int) Constant 4
             104:             TypePointer PushConstant 6(float)
         4(main):           2 Function None 3
               5:             Label
  9(world_light):      8(ptr) Variable Function
   34(world_pos):      8(ptr) Variable Function
49(world_normal):      8(ptr) Variable Function
   63(light_dir):      8(ptr) Variable Function
  68(brightness):     67(ptr) Variable Function
              18:     17(ptr) AccessChain 14(params) 16
              19:          11 Load 18
              22:     21(ptr) AccessChain 14(params) 20
              23:    7(fvec3) Load 22
              25:    6(float) CompositeExtract 23 0
              26:    6(float) CompositeExtract 23 1
              27:    6(float) CompositeExtract 23 2
              28:   10(fvec4) CompositeConstruct 25 26 27 24
              29:   10(fvec4) MatrixTimesVector 19 28
              30:    6(float) CompositeExtract 29 0
              31:    6(float) CompositeExtract 29 1
              32:    6(float) CompositeExtract 29 2
              33:    7(fvec3) CompositeConstruct 30 31 32
                              Store 9(world_light) 33
              35:     17(ptr) AccessChain 14(params) 16
              36:          11 Load 35
              39:    7(fvec3) Load 38(in_pos)
              40:    6(float) CompositeExtract 39 0
              41:    6(float) CompositeExtract 39 1
              42:    6(float) CompositeExtract 39 2
              43:   10(fvec4) CompositeConstruct 40 41 42 24
              44:   10(fvec4) MatrixTimesVector 36 43
              45:    6(float) CompositeExtract 44 0
              46:    6(float) CompositeExtract 44 1
              47:    6(float) CompositeExtract 44 2
              48:    7(fvec3) CompositeConstruct 45 46 47
                              Store 34(world_pos) 48
              50:     17(ptr) AccessChain 14(params) 16
              51:          11 Load 50
              53:   10(fvec4) CompositeExtract 51 0
              54:    7(fvec3) VectorShuffle 53 53 0 1 2
              55:   10(fvec4) CompositeExtract 51 1
              56:    7(fvec3) VectorShuffle 55 55 0 1 2
              57:   10(fvec4) CompositeExtract 51 2
              58:    7(fvec3) VectorShuffle 57 57 0 1 2
              59:          52 CompositeConstruct 54 56 58
              61:    7(fvec3) Load 60(in_normal)
              62:    7(fvec3) MatrixTimesVector 59 61
                              Store 49(world_normal) 62
              64:    7(fvec3) Load 9(world_light)
              65:    7(fvec3) Load 34(world_pos)
              66:    7(fvec3) FSub 64 65
                              Store 63(light_dir) 66
              69:    7(fvec3) Load 63(light_dir)
              70:    7(fvec3) Load 49(world_normal)
              71:    6(float) Dot 69 70
              72:    7(fvec3) Load 63(light_dir)
              73:    6(float) ExtInst 1(GLSL.std.450) 66(Length) 72
              74:    6(float) FDiv 71 73
              75:    7(fvec3) Load 49(world_normal)
              76:    6(float) ExtInst 1(GLSL.std.450) 66(Length) 75
              77:    6(float) FDiv 74 76
                              Store 68(brightness) 77
              78:    6(float) Load 68(brightness)
              79:    6(float) ExtInst 1(GLSL.std.450) 4(FAbs) 78
                              Store 68(brightness) 79
              84:     17(ptr) AccessChain 14(params) 83
              85:          11 Load 84
              86:    7(fvec3) Load 34(world_pos)
              87:    6(float) CompositeExtract 86 0
              88:    6(float) CompositeExtract 86 1
              89:    6(float) CompositeExtract 86 2
              90:   10(fvec4) CompositeConstruct 87 88 89 24
              91:   10(fvec4) MatrixTimesVector 85 90
              93:     92(ptr) AccessChain 82 20
                              Store 93 91
              97:     21(ptr) AccessChain 14(params) 96
              98:    7(fvec3) Load 97
              99:    6(float) Load 68(brightness)
             100:    7(fvec3) VectorTimesScalar 98 99
                              Store 95(color) 100
             105:    104(ptr) AccessChain 14(params) 103
             106:    6(float) Load 105
                              Store 102(alpha) 106
                              Return
                              FunctionEnd
#endif

static const uint32_t Hologram_push_constant_vert[726] = {
    0x07230203, 0x00010000, 0x00080007, 0x0000006b,
    0x00000000, 0x00020011, 0x00000001, 0x0006000b,
    0x00000001, 0x4c534c47, 0x6474732e, 0x3035342e,
    0x00000000, 0x0003000e, 0x00000000, 0x00000001,
    0x000a000f, 0x00000000, 0x00000004, 0x6e69616d,
    0x00000000, 0x00000026, 0x0000003c, 0x00000052,
    0x0000005f, 0x00000066, 0x00030003, 0x00000001,
    0x00000136, 0x00040005, 0x00000004, 0x6e69616d,
    0x00000000, 0x00050005, 0x00000009, 0x6c726f77,
    0x696c5f64, 0x00746867, 0x00050005, 0x0000000c,
    0x61726170, 0x6c625f6d, 0x006b636f, 0x00060006,
    0x0000000c, 0x00000000, 0x6867696c, 0x6f705f74,
    0x00000073, 0x00060006, 0x0000000c, 0x00000001,
    0x6867696c, 0x6f635f74, 0x00726f6c, 0x00050006,
    0x0000000c, 0x00000002, 0x65646f6d, 0x0000006c,
    0x00070006, 0x0000000c, 0x00000003, 0x77656976,
    0x6f72705f, 0x7463656a, 0x006e6f69, 0x00050006,
    0x0000000c, 0x00000004, 0x68706c61, 0x00000061,
    0x00040005, 0x0000000e, 0x61726170, 0x0000736d,
    0x00050005, 0x00000022, 0x6c726f77, 0x6f705f64,
    0x00000073, 0x00040005, 0x00000026, 0x705f6e69,
    0x0000736f, 0x00060005, 0x00000031, 0x6c726f77,
    0x6f6e5f64, 0x6c616d72, 0x00000000, 0x00050005,
    0x0000003c, 0x6e5f6e69, 0x616d726f, 0x0000006c,
    0x00050005, 0x0000003f, 0x6867696c, 0x69645f74,
    0x00000072, 0x00050005, 0x00000044, 0x67697262,
    0x656e7468, 0x00007373, 0x00060005, 0x00000050,
    0x505f6c67, 0x65567265, 0x78657472, 0x00000000,
    0x00060006, 0x00000050, 0x00000000, 0x505f6c67,
    0x7469736f, 0x006e6f69, 0x00070006, 0x00000050,
    0x00000001, 0x505f6c67, 0x746e696f, 0x657a6953,
    0x00000000, 0x00030005, 0x00000052, 0x00000000,
    0x00040005, 0x0000005f, 0x6f6c6f63, 0x00000072,
    0x00040005, 0x00000066, 0x68706c61, 0x00000061,
    0x00050048, 0x0000000c, 0x00000000, 0x00000023,
    0x00000000, 0x00050048, 0x0000000c, 0x00000001,
    0x00000023, 0x00000010, 0x00040048, 0x0000000c,
    0x00000002, 0x00000005, 0x00050048, 0x0000000c,
    0x00000002, 0x00000023, 0x00000020, 0x00050048,
    0x0000000c, 0x00000002, 0x00000007, 0x00000010,
    0x00040048, 0x0000000c, 0x00000003, 0x00000005,
    0x00050048, 0x0000000c, 0x00000003, 0x00000023,
    0x00000060, 0x00050048, 0x0000000c, 0x00000003,
    0x00000007, 0x00000010, 0x00050048, 0x0000000c,
    0x00000004, 0x00000023, 0x000000a0, 0x00030047,
    0x0000000c, 0x00000002, 0x00040047, 0x00000026,
    0x0000001e, 0x00000000, 0x00040047, 0x0000003c,
    0x0000001e, 0x00000001, 0x00050048, 0x00000050,
    0x00000000, 0x0000000b, 0x00000000, 0x00050048,
    0x00000050, 0x00000001, 0x0000000b, 0x00000001,
    0x00030047, 0x00000050, 0x00000002, 0x00040047,
    0x0000005f, 0x0000001e, 0x00000000, 0x00040047,
    0x00000066, 0x0000001e, 0x00000001, 0x00020013,
    0x00000002, 0x00030021, 0x00000003, 0x00000002,
    0x00030016, 0x00000006, 0x00000020, 0x00040017,
    0x00000007, 0x00000006, 0x00000003, 0x00040020,
    0x00000008, 0x00000007, 0x00000007, 0x00040017,
    0x0000000a, 0x00000006, 0x00000004, 0x00040018,
    0x0000000b, 0x0000000a, 0x00000004, 0x0007001e,
    0x0000000c, 0x00000007, 0x00000007, 0x0000000b,
    0x0000000b, 0x00000006, 0x00040020, 0x0000000d,
    0x00000009, 0x0000000c, 0x0004003b, 0x0000000d,
    0x0000000e, 0x00000009, 0x00040015, 0x0000000f,
    0x00000020, 0x00000001, 0x0004002b, 0x0000000f,
    0x00000010, 0x00000002, 0x00040020, 0x00000011,
    0x00000009, 0x0000000b, 0x0004002b, 0x0000000f,
    0x00000014, 0x00000000, 0x00040020, 0x00000015,
    0x00000009, 0x00000007, 0x0004002b, 0x00000006,
    0x00000018, 0x3f800000, 0x00040020, 0x00000025,
    0x00000001, 0x00000007, 0x0004003b, 0x00000025,
    0x00000026, 0x00000001, 0x00040018, 0x00000034,
    0x00000007, 0x00000003, 0x0004003b, 0x00000025,
    0x0000003c, 0x00000001, 0x00040020, 0x00000043,
    0x00000007, 0x00000006, 0x0004001e, 0x00000050,
    0x0000000a, 0x00000006, 0x00040020, 0x00000051,
    0x00000003, 0x00000050, 0x0004003b, 0x00000051,
    0x00000052, 0x00000003, 0x0004002b, 0x0000000f,
    0x00000053, 0x00000003, 0x00040020, 0x0000005c,
    0x00000003, 0x0000000a, 0x00040020, 0x0000005e,
    0x00000003, 0x00000007, 0x0004003b, 0x0000005e,
    0x0000005f, 0x00000003, 0x0004002b, 0x0000000f,
    0x00000060, 0x00000001, 0x00040020, 0x00000065,
    0x00000003, 0x00000006, 0x0004003b, 0x00000065,
    0x00000066, 0x00000003, 0x0004002b, 0x0000000f,
    0x00000067, 0x00000004, 0x00040020, 0x00000068,
    0x00000009, 0x00000006, 0x00050036, 0x00000002,
    0x00000004, 0x00000000, 0x00000003, 0x000200f8,
    0x00000005, 0x0004003b, 0x00000008, 0x00000009,
    0x00000007, 0x0004003b, 0x00000008, 0x00000022,
    0x00000007, 0x0004003b, 0x00000008, 0x00000031,
    0x00000007, 0x0004003b, 0x00000008, 0x0000003f,
    0x00000007, 0x0004003b, 0x00000043, 0x00000044,
    0x00000007, 0x00050041, 0x00000011, 0x00000012,
    0x0000000e, 0x00000010, 0x0004003d, 0x0000000b,
    0x00000013, 0x00000012, 0x00050041, 0x00000015,
    0x00000016, 0x0000000e, 0x00000014, 0x0004003d,
    0x00000007, 0x00000017, 0x00000016, 0x00050051,
    0x00000006, 0x00000019, 0x00000017, 0x00000000,
    0x00050051, 0x00000006, 0x0000001a, 0x00000017,
    0x00000001, 0x00050051, 0x00000006, 0x0000001b,
    0x00000017, 0x00000002, 0x00070050, 0x0000000a,
    0x0000001c, 0x00000019, 0x0000001a, 0x0000001b,
    0x00000018, 0x00050091, 0x0000000a, 0x0000001d,
    0x00000013, 0x0000001c, 0x00050051, 0x00000006,
    0x0000001e, 0x0000001d, 0x00000000, 0x00050051,
    0x00000006, 0x0000001f, 0x0000001d, 0x00000001,
    0x00050051, 0x00000006, 0x00000020, 0x0000001d,
    0x00000002, 0x00060050, 0x00000007, 0x00000021,
    0x0000001e, 0x0000001f, 0x00000020, 0x0003003e,
    0x00000009, 0x00000021, 0x00050041, 0x00000011,
    0x00000023, 0x0000000e, 0x00000010, 0x0004003d,
    0x0000000b, 0x00000024, 0x00000023, 0x0004003d,
    0x00000007, 0x00000027, 0x00000026, 0x00050051,
    0x00000006, 0x00000028, 0x00000027, 0x00000000,
    0x00050051, 0x00000006, 0x00000029, 0x00000027,
    0x00000001, 0x00050051, 0x00000006, 0x0000002a,
    0x00000027, 0x00000002, 0x00070050, 0x0000000a,
    0x0000002b, 0x00000028, 0x00000029, 0x0000002a,
    0x00000018, 0x00050091, 0x0000000a, 0x0000002c,
    0x00000024, 0x0000002b, 0x00050051, 0x00000006,
    0x0000002d, 0x0000002c, 0x00000000, 0x00050051,
    0x00000006, 0x0000002e, 0x0000002c, 0x00000001,
    0x00050051, 0x00000006, 0x0000002f, 0x0000002c,
    0x00000002, 0x00060050, 0x00000007, 0x00000030,
    0x0000002d, 0x0000002e, 0x0000002f, 0x0003003e,
    0x00000022, 0x00000030, 0x00050041, 0x00000011,
    0x00000032, 0x0000000e, 0x00000010, 0x0004003d,
    0x0000000b, 0x00000033, 0x00000032, 0x00050051,
    0x0000000a, 0x00000035, 0x00000033, 0x00000000,
    0x0008004f, 0x00000007, 0x00000036, 0x00000035,
    0x00000035, 0x00000000, 0x00000001, 0x00000002,
    0x00050051, 0x0000000a, 0x00000037, 0x00000033,
    0x00000001, 0x0008004f, 0x00000007, 0x00000038,
    0x00000037, 0x00000037, 0x00000000, 0x00000001,
    0x00000002, 0x00050051, 0x0000000a, 0x00000039,
    0x00000033, 0x00000002, 0x0008004f, 0x00000007,
    0x0000003a, 0x00000039, 0x00000039, 0x00000000,
    0x00000001, 0x00000002, 0x00060050, 0x00000034,
    0x0000003b, 0x00000036, 0x00000038, 0x0000003a,
    0x0004003d, 0x00000007, 0x0000003d, 0x0000003c,
    0x00050091, 0x00000007, 0x0000003e, 0x0000003b,
    0x0000003d, 0x0003003e, 0x00000031, 0x0000003e,
    0x0004003d, 0x00000007, 0x00000040, 0x00000009,
    0x0004003d, 0x00000007, 0x00000041, 0x00000022,
    0x00050083, 0x00000007, 0x00000042, 0x00000040,
    0x00000041, 0x0003003e, 0x0000003f, 0x00000042,
    0x0004003d, 0x00000007, 0x00000045, 0x0000003f,
    0x0004003d, 0x00000007, 0x00000046, 0x00000031,
    0x00050094, 0x00000006, 0x00000047, 0x00000045,
    0x00000046, 0x0004003d, 0x00000007, 0x00000048,
    0x0000003f, 0x0006000c, 0x00000006, 0x00000049,
    0x00000001, 0x00000042, 0x00000048, 0x00050088,
    0x00000006, 0x0000004a, 0x00000047, 0x00000049,
    0x0004003d, 0x00000007, 0x0000004b, 0x00000031,
    0x0006000c, 0x00000006, 0x0000004c, 0x00000001,
    0x00000042, 0x0000004b, 0x00050088, 0x00000006,
    0x0000004d, 0x0000004a, 0x0000004c, 0x0003003e,
    0x00000044, 0x0000004d, 0x0004003d, 0x00000006,
    0x0000004e, 0x00000044, 0x0006000c, 0x00000006,
    0x0000004f, 0x00000001, 0x00000004, 0x0000004e,
    0x0003003e, 0x00000044, 0x0000004f, 0x00050041,
    0x00000011, 0x00000054, 0x0000000e, 0x00000053,
    0x0004003d, 0x0000000b, 0x00000055, 0x00000054,
    0x0004003d, 0x00000007, 0x00000056, 0x00000022,
    0x00050051, 0x00000006, 0x00000057, 0x00000056,
    0x00000000, 0x00050051, 0x00000006, 0x00000058,
    0x00000056, 0x00000001, 0x00050051, 0x00000006,
    0x00000059, 0x00000056, 0x00000002, 0x00070050,
    0x0000000a, 0x0000005a, 0x00000057, 0x00000058,
    0x00000059, 0x00000018, 0x00050091, 0x0000000a,
    0x0000005b, 0x00000055, 0x0000005a, 0x00050041,
    0x0000005c, 0x0000005d, 0x00000052, 0x00000014,
    0x0003003e, 0x0000005d, 0x0000005b, 0x00050041,
    0x00000015, 0x00000061, 0x0000000e, 0x00000060,
    0x0004003d, 0x00000007, 0x00000062, 0x00000061,
    0x0004003d, 0x00000006, 0x00000063, 0x00000044,
    0x0005008e, 0x00000007, 0x00000064, 0x00000062,
    0x00000063, 0x0003003e, 0x0000005f, 0x00000064,
    0x00050041, 0x00000068, 0x00000069, 0x0000000e,
    0x00000067, 0x0004003d, 0x00000006, 0x0000006a,
    0x00000069, 0x0003003e, 0x00000066, 0x0000006a,
    0x000100fd, 0x00010038,
};
