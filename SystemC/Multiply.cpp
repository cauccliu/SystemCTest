#include<systemc.h>
#include<tlm.h>
#include <sstream>
using namespace sc_core;

#define VEC_WIDTH 4
#define VEC_NUM 32
const int WIDTH = 32;

// 端口定义、功能描述与构造函数。
// 两个类型为bool的控制端口：clk和rst_n。分别是时钟和复位端口；
SC_MODULE(vector_mul){ // 实现功能为两个数组的乘法求和
    
    SC_CTOR(vector_mul){
        SC_METHOD(compute_vector_mul); // METHOD是一种阻塞式的功能进程，当这个进程被敏感列表触发之后，获取仿真控制权开始运行，直到运行完成，将控制权返回SystemC仿真内核。
        sensitive << clk.pos();// sensitive <<表示事件（电平）敏感，一般用于组合逻辑建模；
        sensitive << rst_n.neg(); // sensitive_pos <<和sensitive_neg <<分别为正跳变敏感和负跳变敏感，一般用于时序逻辑建模。
    }
    
    sc_in<bool> clk;
    sc_in<bool> rst_n;
    sc_in<sc_int<WIDTH>>         vec1[VEC_WIDTH];
    sc_in<sc_int<WIDTH> >        vec2[VEC_WIDTH];
    sc_out<sc_int<WIDTH * 2>>    vec_o;

    void compute_vector_mul(void){
        int temp = 0;
        // 使用.read()读取输入端口的数据并使用.write()向输出端口写入数据。
        if (rst_n.read() == false) {
            vec_o.write(0);
            return;
        }

        for(int i =0; i<VEC_WIDTH;++i){
            temp = temp + vec1[i].read() * vec2[i].read();
        }
        vec_o.write(temp);
    }
    
};

// 矩阵乘法 使用多个vector_mul实例
SC_MODULE(matrix_vector_mul){
    SC_CTOR(matrix_vector_mul){
        std::ostringstream   pe_name; //使用std::ostringstream生成唯一不重复的实例名，再用c_str()转为构造函数指定的类型。
        for(int i=0;i<VEC_NUM;++i){
            pe_name << "pe" << i;
            pe[i] = new vector_mul(pe_name.str().c_str());
            pe[i]->clk(clk);
            pe[i]->rst_n(rst_n);

            for (int j = 0; j < VEC_WIDTH; ++j) {
                pe[i]->vec1[j](matrix[i][j]);
                pe[i]->vec2[j](vector_in[j]);
            }

            pe[i]->vec_o(vector_out[i]);
            pe_name.str("");
        }
    }

    sc_in<bool> clk;
    sc_in<bool> rst_n;
    sc_in<sc_int<WIDTH> >        matrix[VEC_NUM][VEC_WIDTH];
    sc_in<sc_int<WIDTH> >        vector_in[VEC_WIDTH];
    sc_out<sc_int<WIDTH * 2> >   vector_out[VEC_NUM];

    vector_mul     *pe[VEC_NUM];
};

// 生成输入数据和复位信号。
SC_MODULE(driver){
    SC_CTOR(driver) {
        SC_THREAD(generate_input);
        sensitive << clk.neg();
        SC_THREAD(generate_reset);
    };

    sc_in<bool> clk;
    sc_out<bool> rst_n;
    sc_out<sc_int<WIDTH> > mat[VEC_NUM][VEC_WIDTH];
    sc_out<sc_int<WIDTH> > vec[VEC_WIDTH];

    void generate_input(void){
        for (int i = 0; i < VEC_WIDTH; ++i) {
            for (int j = 0; j < VEC_NUM; ++j) {
                mat[j][i].write(rand() % ((int)pow(2,WIDTH) - 1));
            }
            vec[i].write(rand() % ((int)pow(2, WIDTH) - 1));
        }


        // THREAD宏与METHOD的区别是这一种功能进程在仿真开始时运行，碰到wait()跳出，直到敏感列表中的信号再次触发这一进程，从上次跳出的wait()处继续运行，因此这种进程可以使用循环体包括wait()的无限循环。
        while(1) {
            wait(); // 暂停线程的执行，直到下一个事件发生
            for (int i = 0; i < VEC_WIDTH; ++i) {
                for (int j = 0; j < VEC_NUM; ++j) {
                    mat[j][i].write(rand() % ((int)pow(2, WIDTH) - 1));
                }
                vec[i].write(rand() % ((int)pow(2, WIDTH) - 1));
            }
        }
    }

    void generate_reset(void){
        rst_n.write(1);
        wait(1,SC_NS); // 除了使用wait()阻塞运行外，还可以使用wait(<times>,SC_NS);将执行延迟指定的时钟周期，如rst_n信号的实现，使用多个wait(<times>,SC_NS)延迟执行。 
        rst_n.write(0);
        wait(1,SC_NS);
        rst_n.write(1);
    }
};

int sc_main(int argc,char* argv[]){
    sc_clock clk("clk",10,SC_NS); // 声明始终周期为10纳秒。
    sc_signal<bool> rst_n; // bool值信号，表示复位信号（低电平有效）。
    
    // 声明了一个名为 mat 的矩阵信号，类型为 sc_int，位宽为 WIDTH，维度为 VEC_NUM x VEC_WIDTH。
    // 声明了一个名为 vec 的向量信号，类型为 sc_int，位宽为 WIDTH，维度为 VEC_WIDTH。
    sc_signal<sc_int<WIDTH> > mat[VEC_NUM][VEC_WIDTH],vec[VEC_WIDTH]; 
    sc_signal<sc_int<WIDTH * 2> >vec_o[VEC_NUM]; // 声明了一个名为 vec_o 的输出向量信号，类型为 sc_int，位宽为 WIDTH * 2，维度为 VEC_NUM。

    sc_trace_file *fp; // Create VCD file
    fp = sc_create_vcd_trace_file("wave");// open(fp), create wave.vcd file
    fp->set_time_unit(1, SC_NS);        // set tracing resolution to ns

    matrix_vector_mul dut("dut");
    dut.clk(clk);
    dut.rst_n(rst_n);

    for (int i = 0; i < VEC_NUM; ++i) {
        for (int j = 0; j < VEC_WIDTH; ++j) {
            dut.matrix[i][j](mat[i][j]); // 矩阵是sc_in这都是信号，需要用sc_signal的数据进行初始化。
            // matrix_vector_mul 模块的输入和输出端口被连接到信号上，例如 mat、vec 和 vec_o。这些信号用于传递数据给模块，并从模块中接收数据。
        }
    }
    for (int i = 0; i < VEC_WIDTH; ++i) {
        dut.vector_in[i](vec[i]);
    }
    for (int i = 0; i < VEC_NUM; ++i) {
        dut.vector_out[i](vec_o[i]);
    }

    driver d("dri");
    d.clk(clk);
    d.rst_n(rst_n);
    for (int i = 0; i < VEC_WIDTH; ++i) {
        for (int j = 0; j < VEC_NUM; ++j) {
            d.mat[j][i](mat[j][i]);
        }
        d.vec[i](vec[i]);
    }

    sc_trace(fp,clk,"clk");
    sc_trace(fp,rst_n,"rst_n");
    for (int i = 0; i < VEC_NUM; ++i) {
        for (int j = 0; j < VEC_WIDTH; ++j) {
            std::ostringstream mat_name;
            mat_name << "matrix(" << i << "," << j << ")";
            sc_trace(fp,mat[i][j],mat_name.str());
            mat_name.str("");
        }
    }

    for (int i = 0; i < VEC_WIDTH; ++i) {
        std::ostringstream stream1;
        stream1 << "vec(" << i << ")";
        sc_trace(fp,vec[i],stream1.str());
        stream1.str("");
    }

    for (int i = 0; i < VEC_NUM; ++i) {
        std::ostringstream out_name;
        out_name << "dout(" << i << ")";
        sc_trace(fp,vec_o[i],out_name.str());
        out_name.str("");
    }
 
    sc_start(10,SC_NS);
 
    sc_close_vcd_trace_file(fp);        // close(fp)
    return 0;
}
