package me.ycy.verilog.vpi

import java.util.concurrent._

class Signal[T <: VpiValue](val handle: Long)(m: Manifest[T]) {
  def getValue(): T = ScalaVpi.getValue(handle)(m)
  def setValue(value: T) = ScalaVpi.putValue(handle, value)
  def withValue(func: T => T) = setValue(func(getValue()))
  def withInPlace(func: T => Unit) = {
    val v: T = getValue() // NOTE: here v might be Array
    func(v)
    setValue(v)
  }
}

// subclass of Port should have 1st type parameter as VpiValue
sealed trait Port

case class Input[T <: VpiValue](signal: Signal[T]) extends Port {
  def getValue(): T = signal.getValue()
}

case class Output[T <: VpiValue](signal: Signal[T]) extends Port {
  def setValue(v: T) = signal.setValue(v)
}

case class Inout[T <: VpiValue](signal: Signal[T]) extends Port {
  def getValue(): T = signal.getValue()
  def setValue(v: T) = signal.setValue(v)
  def withValue(func: T => T) = signal.withValue(func)
  def withInPlace(func: T => Unit) = signal.withInPlace(func)
}


object Helpers {
  implicit def tosvec(v: VpiValue.Vector): Vector = {
    new Vector(v.vector)
  }

  implicit def fromsvec(v: Vector): VpiValue.Vector = {
    new VpiValue.Vector(v.ary)
  }

  implicit def scalar2bool(s: VpiValue.Scalar): Boolean = {
    s.scalar == 1
  }

  implicit def bool2scalar(s: Boolean): VpiValue.Scalar = {
    new VpiValue.Scalar(if (s) 1 else 0)
  }

  def vec(v: Long, size: Int): Vector = {
    val ary = Array.ofDim[Int](size)
    for (i <- 0 until size) {
      if ((v & (1L << i)) != 0) ary(i) = 1 else ary(i) = 0
    }
    new Vector(ary)
  }

  class Vector(val ary: Array[Int]) {
    val size = ary.size

    def apply(hi: Int, lo: Int): Vector = {
      if ((hi >= size) || (lo < 0) || (hi < lo)) {
        throw new RuntimeException(
          "index illegal: " +
          "Hi = " + hi + ", Lo = " + lo + ", size = " + size
        )
      }

      val ary1 = Array.ofDim[Int](hi - lo + 1)
      for (i <- lo until hi + 1) { ary1(i - lo) = ary(i) }
      new Vector(ary1)
    }

    def l(): Long = {
      if (ary(size - 1) != 0) {
        ul() - math.pow(2, size).toLong
      }
      else {
        ul()
      }
    }
  
    def ul(): Long = {
      var r = 0l
      for (i <- size - 1 until -1 by -1) {
        r *= 2
        r += ary(i)
      }
      r
    }

    def i(): Int = l().toInt
    def ui(): Int = ul().toInt

    def ++(v: Vector): Vector = {
      val p = new VpiValue.Vector(v.size + size)
      p.vector = v.ary ++ ary
      p
    }

    override def toString(): String = ary.toList.toString
  }

}

object ScalaVpi {
  private[this] val vpi = Vpi.getInstance

  def registerSimpleTask(name: String, func: () ⇒ Unit): Unit = {
    SysTask.register(name, new Callable[Void] {
      def call(): Void = { func(); null }
    })
  }

  def registerTask[T](name: String)(task: T)(
    implicit m: Manifest[T]
  ): Unit = {
    def err(msg: String) = System.err.println("ERROR, registered task " +
      name + " runtime error, " + msg)

    // check task type
    if (!m.erasure.getName.matches("^scala.Function\\d+")) {
      err(m.erasure.getName + " is not function.")
      return
    }

    // do not care about return type of func
    val args = m.typeArguments.dropRight(1)

    // check task argument type
    for (a <- args) {
      if (!classOf[Port].isAssignableFrom(a.erasure)) {
        err(a.toString + " is not subtype of Port.")
        return
      }

      if ((a.typeArguments.size == 0) ||
          (!classOf[VpiValue].isAssignableFrom(a.typeArguments(0).erasure))) {
        err(a.toString + " should take VpiValue as 1st type parameter.")
      }
    }

    // get all argument handle
    def getHandles(): List[Long] = {
      var ret = List[Long]()
      val tfh: Long = vpi.handle(VpiConst.ObjectCode.SysTfCall, 0)
      //System.out.println("tfh -> " + tfh);
      val iter: Long = vpi.iterate(VpiConst.ObjectCode.Argument, tfh);
      //System.out.println("iter -> " + iter);

      // drop first argument, which is jvm task registration name
      var argh: Long = vpi.scan(iter) 
      while (argh != 0) {
        argh = vpi.scan(iter)
        if (argh != 0) {
          ret ::= argh
        }
      }

      ret.reverse
    }

    // assume handle number already checked
    def preparePorts(handles: List[Long]): List[Port] =
      handles zip args map {x => {
        val tp = x._2.typeArguments(0).asInstanceOf[Manifest[VpiValue]]
        val sig: Signal[_ <: VpiValue] = new Signal(x._1)(tp)
        x._2.erasure.getSimpleName match {
          case "Input" ⇒ Input(sig)
          case "Output" ⇒ Output(sig)
          case "Inout" ⇒ Inout(sig)
          case _ ⇒ throw(new RuntimeException("Not support port type."))
        }
      }}


    val cb = new Callable[Void] {
      def call(): Void = {
        try {
          val handles = getHandles()

          if (handles == null) {
            err("fail to get argument handles.")
            return null
          }

          if (handles.size < args.size) {
            err("argument size(" + handles.size + 
                ") from verilog less than " +
                args.size)
            return null
          }

          val ports = preparePorts(handles)
          val applyMethod = 
            task.getClass.getMethod("apply", ports.map(_.getClass): _*)
          applyMethod.invoke(task, ports: _*)
        }
        catch {
          case e => e.printStackTrace
        }
        null
      }
    }

    SysTask.register(name, cb)
  }

  // return null if not support
  def getValue[T <: VpiValue](handle: Long)(implicit m: Manifest[T]): T = {
    import VpiConst.ValueType._

    val tbl: Map[Manifest[_], Int] = Map(
      manifest[VpiValue.Scalar] → ScalarVal,
      manifest[VpiValue.Int] → IntVal,
      manifest[VpiValue.Real] → RealVal,
      manifest[VpiValue.String] → StringVal,
      manifest[VpiValue.Vector] → VectorVal
    )

    tbl.get(m) match {
      case Some(fmt) ⇒ vpi.get_value(handle, fmt).asInstanceOf[T]
      case None ⇒ null.asInstanceOf[T]
    }
  }

  def putValue(
    handle: Long, value: VpiValue,
    time: VpiValue.Time = null,
    flags: Int = VpiConst.DelayMode.NoDelay
  ): Unit =
    vpi.put_value(handle, value, time, flags)

}
