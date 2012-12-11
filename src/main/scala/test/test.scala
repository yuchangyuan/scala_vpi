package test

import java.util.concurrent._
import scala.swing._
import javax.swing.UIManager
import java.util.concurrent.LinkedBlockingQueue
import me.ycy.verilog.vpi._

object TestUI {
  UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName())

  val dataQueue = new LinkedBlockingQueue[Int]

  object Gui extends Frame {
    import javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE
    peer.setDefaultCloseOperation(DO_NOTHING_ON_CLOSE)

    val label = new Label

    contents = new BoxPanel(Orientation.Vertical) {
      contents += new FlowPanel { contents += label }
      contents += new FlowPanel {
        contents += new Button {
          action = Action("data++") { dataQueue.put(1) }
        }
        contents += new Button {
          action = Action("data--") { dataQueue.put(-1) }
        }
      }
      border = Swing.EmptyBorder(5, 5, 5, 5)
    }

    title = "data"
    preferredSize = new Dimension(250, 100)

    repaint
    pack
  }

  def incDec(i: Inout[VpiValue.Int]): Unit = {
    Gui.label.text = "data = " + i.getValue.integer

    val data = dataQueue.take()

    i.withValue {v ⇒ {
      new VpiValue.Int(v.integer + data)
    }}
  }

  def testVec(i: Inout[VpiValue.Vector]): Unit = {
    i.withInPlace { v => {
      println("size = " + v.vector.size)
      println(v.vector.toList)
      for (k <- 0 until v.vector.size) {
        v.vector(k) = k % 4
      }
    }}
  }

  def testReal(r: Inout[VpiValue.Real]): Unit = r.withInPlace { v => {
    println("real in = " + v.real)
    v.real = 2.345
  }}

  def init = {
    ScalaVpi.registerTask("inc_dec")(incDec _)
    ScalaVpi.registerTask("test_vec")(testVec _)
    ScalaVpi.registerTask("test_real")(testReal _)
    Gui.visible = true
  }
}
