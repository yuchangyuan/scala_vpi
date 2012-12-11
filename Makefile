include utils.mk

CFLAGS := $(VLOG_CFLAGS) $(JVM_CFLAGS)
LIBS := $(VLOG_LIBS) $(JVM_LIBS)

VLOG = iverilog

JAVA_SRC = $(wildcard src/main/java/me/ycy/verilog/vpi/*.java)
SCALA_SRC  = $(wildcard src/main/scala/me/ycy/verilog/vpi/*.scala)
SCALA_SRC += $(wildcard src/main/scala/test/*.scala)


all: target/sim.vvp target/scala_vpi.vpi target/me/ycy/verilog/vpi/ScalaVpi.class

target/scala_vpi.vpi: src_gen/me_ycy_verilog_vpi_Vpi.h

target/scala_vpi.vpi: src/main/c/scala_vpi.c src/main/c/jni.c
	$(CC) -shared -Isrc_gen $(CFLAGS) -o $@ $^ $(LIBS)

src_gen/me_ycy_verilog_vpi_Vpi.h: target/me/ycy/verilog/vpi/Vpi.class
	rm -f $@
	javah -d src_gen -classpath target me.ycy.verilog.vpi.Vpi

target/sim.vvp: src/main/verilog/tb.v
	$(VLOG) $^ -o $@

target/me/ycy/verilog/vpi/Vpi.class: $(JAVA_SRC)
	javac -Xlint:unchecked -s src/main/java -d target $(JAVA_SRC)

target/me/ycy/verilog/vpi/ScalaVpi.class: $(SCALA_SRC)
	scalac -unchecked -sourcepath src/main/scala -classpath target -d target $(SCALA_SRC)

.PHONY: clean run

run: target/sim.vvp target/scala_vpi.vpi
	cd target; vvp -M. -mscala_vpi sim.vvp

clean:
	rm -rf target/*
