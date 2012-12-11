package me.ycy.verilog.vpi;

final public class VpiConst {
    /* s_vpi_systf_data.type */
    public static class TfType {
        public final static int SysTask = 1;
        public final static int SysFunc = 2;
    }

    /* s_vpi_time.type */
    public static class TimeType {
        public final static int ScaledRealTime = 1;
        public final static int SimTime = 2;
        public final static int SuppressTime = 3;
    }

    /* s_vpi_value.type */
    public static class ValueType {
        public final static int BinStrVal = 1;
        public final static int OctStrVal = 2;
        public final static int DecStrVal = 3;
        public final static int HexStrVal = 4;
        public final static int ScalarVal = 5;
        public final static int IntVal = 6;
        public final static int RealVal = 7;
        public final static int StringVal = 8;
        public final static int VectorVal = 9;
        public final static int StrengthVal = 10;
        public final static int TimeVal = 11;
        public final static int ObjTypeVal = 12;
        public final static int SuppressVal = 13;
    }

    public static class ScalarValue {
        public final static int v0 = 0;
        public final static int v1 = 1;
        public final static int vZ = 2;
        public final static int vX = 3;
        public final static int vH = 4;
        public final static int vL = 5;
        public final static int vDontCare = 6;
    }

    public static class StrengthValue {
        public final static int SupplyDrive =  0x80;
        public final static int StrongDrive =  0x40;
        public final static int PullDrive =    0x20;
        public final static int LargeCharge =  0x10;
        public final static int WeakDrive =    0x08;
        public final static int MediumCharge = 0x04;
        public final static int SmallCharge =  0x02;
        public final static int HiZ =          0x01;
    }

    public static class DelayMode {
        public final static int NoDelay = 1;
        public final static int InertialDelay = 2;
        public final static int TransportDelay = 3;
        public final static int PureTransportDelay = 4;
        public final static int ForceFlag = 5;
        public final static int ReleaseFlag = 6;
    }

    public static class ObjectCode {
        public final static int Constant =     7;
        public final static int Function =    20;
        public final static int IntegerVar =  25;
        public final static int Iterator =    27;
        public final static int Memory =      29;
        public final static int MemoryWord =  30;
        public final static int ModPath =     31;
        public final static int Module =      32;
        public final static int NamedBegin =  33;
        public final static int NamedEvent =  34;
        public final static int NamedFork =   35;
        public final static int Net =         36;
        public final static int Parameter =   41;
        public final static int PartSelect =  42;
        public final static int PathTerm =    43;
        public final static int RealVar =     47;
        public final static int Reg =         48;
        public final static int SysFuncCall = 56;
        public final static int SysTaskCall = 57;
        public final static int Task =        59;
        public final static int TimeVar =     63;
        public final static int NetArray =   114;
        public final static int Index =       78;
        public final static int LeftRange =   79;
        public final static int Parent =      81;
        public final static int RightRange =  83;
        public final static int Scope =       84;
        public final static int SysTfCall =   85;
        public final static int Argument =    89;
        public final static int InternalScope = 92;
        public final static int ModPathIn =     95;
        public final static int ModPathOut =    96;
        public final static int Variables =   100;
        public final static int Expr =        102;
    }

    public static class Prop {
        public final static int Undefined =   (-1);
        public final static int Type =           1;
        public final static int Name =           2;
        public final static int FullName =       3;
        public final static int Size =           4;
        public final static int File =           5;
        public final static int LineNo =         6;
        public final static int TopModule =      7;
        public final static int CellInstance =   8;
        public final static int DefName =        9;
        public final static int TimeUnit =      11;
        public final static int TimePrecision = 12;
        public final static int DefFile =       15;
        public final static int DefLineNo =     16;
        public final static int NetType =       22;
        public final static int Wire =         1;
        public final static int Wand =         2;
        public final static int Wor =          3;
        public final static int Tri =          4;
        public final static int Tri0 =         5;
        public final static int Tri1 =         6;
        public final static int TriReg =       7;
        public final static int TriAnd =       8;
        public final static int TriOr =        9;
        public final static int Supply1 =     10;
        public final static int Supply0 =     11;
        public final static int Array =         28;
        public final static int Edge =          36;
        public static class Edge {
            public final static int NoEdge = 0x00; /* No edge */
            public final static int Edge01 = 0x01; /* 0 --> 1 */
            public final static int Edge10 = 0x02; /* 1 --> 0 */
            public final static int Edge0x = 0x04; /* 0 --> x */
            public final static int Edgex1 = 0x08; /* x --> 1 */
            public final static int Edge1x = 0x10; /* 1 --> x */
            public final static int Edgex0 = 0x20; /* x --> 0 */
            public final static int Posedge = (Edgex1|Edge01|Edge0x);
            public final static int Negedge = (Edgex0|Edge10|Edge1x);
            public final static int AnyEdge = (Posedge|Negedge);
        }
        public final static int ConstType = 40;
        public static class Const {
            public final static int DecConst =    1;
            public final static int RealConst =   2;
            public final static int BinaryConst = 3;
            public final static int OctConst =    4;
            public final static int HexConst =    5;
            public final static int StringConst = 6;
        }
        public final static int FuncType =  44;
        public static class Func {
            public final static int IntFunc =     1;
            public final static int RealFunc =    2;
            public final static int TimeFunc =    3;
            public final static int SizedFunc =   4;
            public final static int SizedSignedFunc = 5;
        }
        public final static int SysFuncType = FuncType;
        public static class SysFunc {
            public final static int SysFuncInt =   Func.IntFunc;
            public final static int SysFuncReal =  Func.RealFunc;
            public final static int SysFuncTime =  Func.TimeFunc;
            public final static int SysFuncSized = Func.SizedFunc;
        }
        public final static int Automatic =      50;
        public final static int ConstantSelect = 53;
        public final static int Signed =         65;
    }

    /* t_cb_data.reason */
    public static class Callback {
        public final static int ValueChange =        1;
        public final static int Stmt =               2;
        public final static int Force =              3;
        public final static int Release =            4;
        public final static int AtStartOfSimTime =   5;
        public final static int ReadWriteSynch =     6;
        public final static int ReadOnlySynch =      7;
        public final static int NextSimTime =        8;
        public final static int AfterDelay =         9;
        public final static int EndOfCompile =      10;
        public final static int StartOfSimulation = 11;
        public final static int EndOfSimulation =   12;
        public final static int Error =             13;
        public final static int TchkViolation =     14;
        public final static int StartOfSave =       15;
        public final static int EndOfSave =         16;
        public final static int StartOfRestart =    17;
        public final static int EndOfRestart =      18;
        public final static int StartOfReset =      19;
        public final static int EndOfReset =        20;
        public final static int EnterInteractive =  21;
        public final static int ExitInteractive =   22;
        public final static int InteractiveScopeChange = 23;
        public final static int UnresolvedSystf =   24;
    }

    /* s_vpi_error_info.state */
    public static class ErrorState {
        public final static int Compile =  1;
        public final static int PLI =      2;
        public final static int Run =      3;
    }

    /* s_vpi_error_info.level */
    public static class ErrorLevel {
        public final static int Notice =    1;
        public final static int Warning =   2;
        public final static int Error =     3;
        public final static int System =    4;
        public final static int Internal =  5;
    }

    /* vpi_control, op */
    public static class Operation {
        /* execute simulator's $stop */
        public final static int Stop = 66;
        /* execute simulator's $finish */
        public final static int Finish = 67;
        /* execute simulator's $reset */
        public final static int Reset = 68;
        /* set simulator's interactive scope */
        public final static int SetInteractiveScope = 69;
    }
}
