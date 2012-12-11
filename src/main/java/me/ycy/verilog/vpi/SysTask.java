package me.ycy.verilog.vpi;

import java.util.*;
import java.util.concurrent.*;

class SysTask {
    private static Map<String, Callable<Void>> map =
        new ConcurrentHashMap<String, Callable<Void>>();

    public static boolean call(String name) {
        Callable<Void> c = map.get(name);
        if (c == null) {
            System.err.println("Task " + name + " not found");
            return false;
        }
        else {
            try {
                c.call();
                return true;
            }
            catch (Exception e) {
                return false;
            }
        }
    }

    public static void register(String name, Callable<Void> task) {
        System.out.println("register task " + name);
        map.put(name, task);
    }

    public static void unregister(String name) {
        map.remove(name);
    }
}
