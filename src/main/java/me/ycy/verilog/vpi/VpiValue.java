package me.ycy.verilog.vpi;

import java.util.*;

public abstract class VpiValue {
    abstract int getFormat();

    // All subtype
    public static class Int extends VpiValue {
        public int integer;

        public Int(int integer) { this.integer = integer; }
        public int getFormat() { return VpiConst.ValueType.IntVal; }
    }

    public static class Scalar extends VpiValue {
    	public int scalar;

        public Scalar(int scalar) { this.scalar = scalar; }
        public int getFormat() { return VpiConst.ValueType.ScalarVal; }
    }

    public static class Real extends VpiValue {
    	public double real;

        public Real(double real) { this.real = real; }
        public int getFormat() { return VpiConst.ValueType.RealVal; }
    }

    public static class String extends VpiValue {
    	public java.lang.String str;

        public String(java.lang.String str) { this.str = str; }
        public int getFormat() { return VpiConst.ValueType.StringVal; }
    }

    public static class Time extends VpiValue {
        public int type;
        public long high;
        public long low;
        public double real;

        public int getFormat() { return VpiConst.ValueType.TimeVal; }

        // NOTE: high & low is UINT32
        public Time(int type, long high, long low, double real) {
            this.type = type;
            this.high = high;
            this.low = low;
            this.real = real;
        }
    }

    public static class Vector extends VpiValue {
    	public int[] vector;

        public int getFormat() { return VpiConst.ValueType.VectorVal; }

        public Vector(int n) {
            vector = new int[n];
            int i;
            for (i = 0; i < n; i++) {
                vector[i] = VpiConst.ScalarValue.vX;
            }
        }

        public Vector(int[] vec) {
            vector = vec;
        }

        public Vector(Collection<Integer> vec) {
            Iterator<Integer> i = vec.iterator();
            vector = new int[vec.size()];

            int idx = 0;
            while (i.hasNext()) {
                vector[idx] = i.next();
                ++idx;
            }
        }

        public void set(int idx, int v) {
            vector[idx] = v;
        }

        public int get(int idx) {
            return vector[idx];
        }
    }

}
