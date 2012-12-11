`timescale 1ns/10ps

module tb;
   integer data;
   reg [35:0] vec;
   
   
   // jvm init
   initial begin
      $jvm_start("-Xmx512M", "-Xms128M");
      $jvm_invoke("test/TestUI", "init");
   end // initial begin

   real r;

   initial begin
      data = 0;
      vec = 36'h1_23xz_z567;
      
      #10;

      $jvm_task("test_vec", vec);
      $display("vec = %b", vec);

      r = 1.234;      
      $jvm_task("test_real", r);
      $display("r = %f", r);

      //$display("data = %d", data);
      $fflush;
      
      repeat (5) begin
         #10 $jvm_task("inc_dec", data);
         $display("data = %d", data);
         $fflush;
      end
   end

endmodule // tb
