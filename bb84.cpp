/************************************************************/
/* File:     bb84.cpp                                       */
/* Function: Simulate the BB84 protocol                     */
/* Author:   Dragon Liu                                     */
/* Date:     2021.05.08                                     */
/* Github:   https://github.com/Dragonliu2018/BB84-protocol */
/************************************************************/
#include <bits/stdc++.h>
using namespace std;

const int maxn = 100; //Alice发送粒子总数
//下面数组编号均从1开始
int p_A[maxn + 1]; //Alice发送的粒子，4种取值：0->0 1->1 2->+ 3->- 
int base_E[maxn + 1]; //Eve选择窃听使用的基，3种取值：0->放过 1->01基 2->+-基
int p_E[maxn + 1]; //Eve的测量结果，5种取值：0->0 1->1 2->+ 3->- 4->放过
int base_B[maxn + 1]; //Bob选择的测量基，2种取值：1->01基 2->+-基
int p_B[maxn + 1]; //Bob的测量结果，4种取值：0->0 1->1 2->+ 3->- 
map<int, string> mmp;//粒子态的映射，便于输出
map<int, string> mmb;//粒子基的映射，便于输出
map<string, int> mmk;//密钥编码映射，便于输出
string com[maxn + 1];//经典通信结果，2种取值：对 弃
int com_suc_num;//经典通信，选择正确测量基的个数
int com_fai_num;//经典通信，选择错误测量基的个数
int E_suc_num;//Eve窃听选择正确基的数量
string hack_public[maxn + 1];//Eve窃取密钥
int E_public_num;//最终共享密钥Eve选择正确基的数量(窃取密钥量)
int public_num;//公开粒子数(即检测窃听)
float public_pre = 0.5;//设定50%的粒子进行公开，按顺序的前半部分
string check_hack[maxn + 1];//检测窃听，2种取值：正确 错误
int public_error_num;//检测窃听错误数
int share_num;//最终共享粒子数
string error_E[maxn + 1];//Eve引发错误
int error_E_num;//Eve引发错误数量
char key_A[maxn + 1];//Alice的密钥
char key_B[maxn + 1];//Bob的密钥
string s_key_A;//字符串类型
string s_key_B;
/* 初始化映射，便于输出 */
void init_mm() {
    mmp[0] = "0";
    mmp[1] = "1";
    mmp[2] = "+";
    mmp[3] = "-";
    mmp[4] = "放过";

    mmb[0] = "放过";
    mmb[1] = "01";
    mmb[2] = "+-";

    mmk["0"] = '0';
    mmk["1"] = '1';
    mmk["+"] = '1';//+态编码为1
    mmk["-"] = '0';//-态编码为0
}

/* 返回自系统开机以来的毫秒数；linux不能直接调用，需要转换*/
unsigned long GetTickCount() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (ts.tv_sec * 1000 + ts.tv_nsec / 1000000);
}

/* Alice发送粒子，随机生成 */
void gen_p_A() {
    srand(GetTickCount()); //设置变化种子
    for (int i = 1; i <= maxn; i++) {
        // cout<< rand()%4<< endl;
        p_A[i] = rand() % 4;//随即决定0 1 + -
    }
}
/* 选择窃听及选择的基 */
void hack_E() {
    srand(GetTickCount()); //设置变化种子
    for (int i = 1; i <= maxn; i++) {
        // cout<< rand()%3<< endl;
        base_E[i] = rand() % 3;//随机决定 放过 01基 +-基
    } 
}
/* Eve的测量结果 */
void cal_E() {
    srand(GetTickCount()); //设置变化种子
    for (int i = 1; i <= maxn; i++) {
        switch (base_E[i]) {
            case 0: p_E[i] = 4; break; //放过
            case 1: p_E[i] = p_A[i] <= 1 ? p_A[i] : (rand() % 2); break; //01基
            case 2: p_E[i] = p_A[i] >= 2 ? p_A[i] : (rand() % 2 + 2); break; //+-基
        }
    }
}
/* Bob选择测量基 */
void chose_B() {
    srand(GetTickCount()); //设置变化种子
    for (int i = 1; i <= maxn; i++) {
        // cout<< rand()%2<< endl;
        base_B[i] = rand() % 2 + 1;//随机决定 01基 +-基
    } 
}
/* Bob的测量结果 */
void cal_B() {
    srand(GetTickCount()); //设置变化种子
    for (int i = 1; i <= maxn; i++) {
        if (base_B[i] == 1) {//01基
            if (p_E[i] == 4) {//Eve放过
                p_B[i] = p_A[i] <= 1 ? p_A[i] : (rand() % 2);
            } else {//Eve进行窃听
                p_B[i] = p_E[i] <= 1 ? p_E[i] : (rand() % 2);
            }
        } else {//+-基
            if (p_E[i] == 4) {//Eve放过
                p_B[i] = p_A[i] >= 2 ? p_A[i] : (rand() % 2 + 2);
            } else {//Eve进行窃听
                p_B[i] = p_E[i] >= 2 ? p_E[i] : (rand() % 2 + 2);
            }
        }
    }
}
/* 计算数据 */
void cal_data() {
    //经典通信
    for (int i = 1; i <= maxn; i++) {
        if (base_B[i] == 1) {//选择01基
            if (p_A[i] <= 1) {//0 1态
                com[i] = "对";
                com_suc_num++;
                if (p_A[i] == p_E[i]) {//Eve选择正确基
                    E_suc_num++;
                }
            } else {//+ -态
                com[i] = "弃";
                com_fai_num++;
            }
        } else {//选择+-基
            if (p_A[i] >= 2) {//+ -态
                com[i] = "对";
                com_suc_num++;
                if (p_A[i] == p_E[i]) {//Eve选择正确基
                    E_suc_num++;
                }
            } else {//0 1态
                com[i] = "弃";
                com_fai_num++;
            }            
        }
    }
    //公开粒子及最终共享密钥
    public_num = com_suc_num * public_pre;//公开粒子数
    public_num = public_num ? public_num : 1;//排除0的情况
    int num = 0;//当前公开的粒子数
    for (int i = 1; i <= maxn; i++) {
        if (com[i] == "对") {
            num++;
            if (num <= public_num) {//公开粒子，检测窃听
                if (p_A[i] == p_B[i]) {
                    check_hack[i] = "正确";
                } else{
                    check_hack[i] = "错误";
                    public_error_num++;
                }
            } else {//选取粒子结束，最终共享密钥
                if (p_A[i] != p_B[i]) {//Eve引发错误
                    error_E[i] = "是";
                    error_E_num++;
                }
                if (p_A[i] == p_E[i]) {
                    hack_public[i] = "是";
                    E_public_num++;
                }
                //最终共享密钥
                key_A[i] = mmk[mmp[p_A[i]]];
                s_key_A += mmk[mmp[p_A[i]]];
                key_B[i] = mmk[mmp[p_B[i]]];
                s_key_B += mmk[mmp[p_B[i]]];
            }
        }
    }
    share_num = com_suc_num - public_num;
    // cout<< public_num<< endl;
}
/* 输出结果 */
void print_data() {
    cout<< "================================================================================================================\n";
	cout<< "| 粒子 | Alice | Eve  | God1 | Bob  | God2 | 经典通信 | Eve引错 | 检测窃听 | Eve窃取密钥 | Alice密钥 | Bob密钥 |\n";
    for (int i = 1; i <= maxn; i++) {
        cout<< "|------+-------+------+------+------+------+----------+---------+----------+-------------+-----------+---------|\n";
        cout<< "|"<< setw(4)<< i             << "  ";
        cout<< "|"<< setw(4)<< mmp[p_A[i]]   << "   ";
        cout<< "|"<< setw(4)<< mmb[base_E[i]]<< "  ";
        cout<< "|"<< setw(4)<< mmp[p_E[i]]   << "  ";
        cout<< "|"<< setw(4)<< mmb[base_B[i]]<< "  ";
        cout<< "|"<< setw(4)<< mmp[p_B[i]]   << "  ";
        cout<< "|"<< setw(7)<< com[i]        << "    ";
        if (error_E[i] != "")
            cout<< "|"<< setw(7)<< error_E[i]<< "   ";
        else
            cout<< "|"<< setw(9)<< "  "; 
        if (check_hack[i] != "") 
            cout<< "|"<< setw(8)<< check_hack[i]<< "    ";
        else
            cout<< "|"<< setw(10)<< "     "; 
        if (hack_public[i] != "") 
            cout<< "|"<< setw(8)<< hack_public[i]<< "      ";
        else  
            cout<< "|"<< setw(13)<< "     ";   
        if (key_A[i])
            cout<< "|"<< setw(6)<< key_A[i]<< "     ";
        else  
            cout<< "|"<< setw(11)<< " ";
        if (key_B[i])
            cout<< "|"<< setw(4)<< key_B[i]<< "     ";
        else  
            cout<< "|"<< setw(9)<< " ";
        cout<< "|"<< endl;
    }
    cout<< "================================================================================================================\n";
    cout<< endl; 
    cout<< "通信后共享初始密钥率(占比总粒子数)："<< com_suc_num<< "/"<< maxn<< " = "
        << double(com_suc_num) / maxn * 100<< "%"<< endl;
    cout<< "Eve窃听选择正确基，即Eve获取的密钥率(占比共享的密钥)："<< E_suc_num<< "/"<< com_suc_num<< " = "
        << double(E_suc_num) / com_suc_num * 100<< "%"<< endl;
    cout<< "检测窃听后的错误率(占比检测窃听粒子数)："<< public_error_num<< "/"<< public_num<< " = "
        << double(public_error_num) / public_num * 100<< "%"<< endl;
    cout<< "最终共享密钥的比例(占比总粒子数)："<< share_num<< "/"<< maxn<< " = "
        << double(share_num) / maxn * 100<< "%"<< endl;
    cout<< "最终共享密钥中，Eve窃听选择正确基，即Eve获取的密钥率："<< E_public_num<< "/"<< share_num<< " = "
        << double(E_public_num) / share_num * 100<< "%"<< endl;
    cout<< "最终共享密钥的错误率(错误粒子占比最终共享粒子)："<< error_E_num<< "/"<< share_num<< " = "
        << double(error_E_num) / share_num * 100<< "%"<< endl;
    if (double(public_error_num) / public_num <= 0.25) {
        cout<< "\n共享密钥成功！"<< endl;
        cout<< "Alice密钥为："<< s_key_A<< endl;
        cout<< "Bob  密钥为："<< s_key_B<< endl;
    } else {
        cout<< "共享密钥失败！"<< endl;
    }
}

int main() {
    init_mm();//初始化映射
    cout<< "Alice随机生成粒子，发送中..."<< endl;
    gen_p_A(); //Alice发送粒子，随机生成
    cout<< "Eve随机选择是否窃听及在哪组基下窃听，窃听中..."<< endl;
    hack_E(); //窃听
    cout<< "God 决定Eve的测量结果..."<< endl;
    cal_E(); //Eve的测量结果
    cout<< "Bob 选择测量基..."<< endl;
    chose_B(); // Bob选择测量基
    cout<< "God 决定Bob的测量结果..."<< endl;
    cal_B();
    cout<< "数据计算..."<< endl;
    cal_data();
    cout<< "输出结果："<< endl;
    print_data();

    return 0;
}