package me.ycy.verilog.vpi;

public class Vpi {

    private static Vpi inst = null;

    private Vpi() {}

    public static synchronized Vpi getInstance() {
        if (inst == null) {
            inst = new Vpi();
        }

        return inst;
    }

    native long handle(int type, long obj);
    native long handle_by_index(long obj, int index);
    native void free_object(long obj);
    native long iterate(int prop, long obj);
    native long scan(long obj);
    native long put_value(long obj, VpiValue value,
                          VpiValue.Time time, int flags);
    native VpiValue get_value(long obj, int format);
    native int get(int prop, long obj);
    native String get_str(int prop, long obj);
}
