#include <gtk/gtk.h>
#include <stdlib.h>
#include "sqlite3.h"

//登录窗口组件
GtkBuilder *builder;
GtkWidget *window;
GtkWidget *label_error;
GtkWidget *btn_login;
GtkWidget *btn_cancle;
GtkWidget *txt_userName;
GtkWidget *txt_password;

//主页窗口组件
GtkWidget *window_home;//主窗体
GtkWidget *lb_user_name;//显示用户名

//新增页面
GtkWidget *add_kd_no;
GtkWidget *add_startAddress;
GtkWidget *add_destAddress;
GtkWidget *add_user;
GtkWidget *add_result;
GtkWidget *add_userPhone;
GtkWidget *add_btnSubmit;


//搜索
GtkWidget *input_search;//搜索框
GtkWidget *btn_search;//搜索按钮
//显示Label控件
GtkWidget *ss_no;
GtkWidget *ss_start;
GtkWidget *ss_dest;
GtkWidget *ss_user;
GtkWidget *ss_phone;
GtkWidget *ss_status;
GtkWidget *ss_null;

//表格布局
GtkWidget *order_list;//订单列表外层容器
GtkWidget *grid;//网格

//刷新按钮点击
void btn_refresh_clicked(){
	gtk_widget_destroy (window_home);
	const gchar *userName = gtk_entry_get_text ((GtkEntry *)txt_userName);
	home_view(userName);
}

//表格布局（在首页中用于显示所有数据）
void init_GtkGrid()
{	
	//创建网格
	grid = gtk_grid_new();
	//添加网格布局到order_list容器上
	gtk_container_set_border_width(GTK_CONTAINER(order_list) , 0);	
	gtk_container_add(GTK_CONTAINER(order_list) , grid);
	gtk_grid_set_column_homogeneous(grid,TRUE);
	
	//初始化表头头信息
    	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("        	ID			") , 0 , 0 , 1 , 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("运单号") , 1 , 0 , 1 , 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("始发地址") , 2 , 0 , 1 , 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("收货地址") , 3 , 0 , 1 , 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("收货人") , 4 , 1 ,0 , 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("联系方式") , 5 , 0 , 1 , 1);
	gtk_grid_attach(GTK_GRID(grid), gtk_label_new("状态") , 6 , 0 , 1 , 1);
				
	//显示所有数据
	gtk_widget_show_all(order_list);
}	

//初始化订单列表
void init_order_list(){
	
	init_GtkGrid();	

	sqlite3 *db = NULL;
	char *sql = NULL;
	int result,nRow,nColumn;
	char *errmsg;
	char **dbResult;

	result = sqlite3_open("atlan.db",&db);
	if(result != SQLITE_OK)
	{
		printf("open error\n");
		return ;
	}
	sql = "select * from goods;";
	result = sqlite3_get_table(db,sql,&dbResult,&nRow,&nColumn,&errmsg);
	if(result == SQLITE_OK && nRow>0)
	{
		int i,j,index;
		index=nColumn;
		for(i=0;i<nRow;i++)
		{
			for(j=0;j<nColumn;j++)
			{	
				GtkLabel *label = gtk_label_new(dbResult[index]);
				gtk_grid_attach(GTK_GRID(grid), label , j , i+1 , 1 , 1);
				if(j==1){
					gtk_label_set_selectable(label,TRUE);//订单号可复制
				}
				gtk_widget_show_all(order_list);
				index++;
			}
		}
		sqlite3_free_table(dbResult);
		
	}
	sqlite3_close(db);
	
}

//搜索按钮点击事件
void btn_search_clicked(){
	sqlite3 *db = NULL;
	int result,nRow,nColumn;
	char *errmsg;
	char **dbResult;
	char sql[100] = "";
	const gchar *good_id = gtk_entry_get_text ((GtkEntry *)input_search);
		//打开数据库
	int ret = sqlite3_open("atlan.db",&db);
	if(ret != SQLITE_OK ){
		printf("数据库打开失败");
		return ;
	}

	//先清空记录
	gtk_label_set_text(GTK_LABEL(ss_no),(const gchar*)"");
	gtk_label_set_text(GTK_LABEL(ss_start),(const gchar*)"");
	gtk_label_set_text(GTK_LABEL(ss_dest),(const gchar*)"");
	gtk_label_set_text(GTK_LABEL(ss_user),(const gchar*)"");
	gtk_label_set_text(GTK_LABEL(ss_phone),(const gchar*)"");
	gtk_label_set_text(GTK_LABEL(ss_status),(const gchar*)"");	

	//从数据库获取记录
	sprintf(sql,"select * from goods where good_id = '%s';",good_id);
	result = sqlite3_get_table(db,sql,&dbResult,&nRow,&nColumn,&errmsg);
	if(result == SQLITE_OK && nRow>0)
	{	gtk_label_set_text(GTK_LABEL(ss_no),dbResult[8]);
		gtk_label_set_text(GTK_LABEL(ss_start),dbResult[9]);
		gtk_label_set_text(GTK_LABEL(ss_dest),dbResult[10]);
		gtk_label_set_text(GTK_LABEL(ss_user),dbResult[11]);
		gtk_label_set_text(GTK_LABEL(ss_phone),dbResult[12]);
		gtk_label_set_text(GTK_LABEL(ss_status),dbResult[13]);
		sqlite3_free_table(dbResult);
	}
	else{	
		gtk_label_set_text(GTK_LABEL(ss_null),(const gchar*)"查询无此记录！");
		printf("errmsg=%s\n",errmsg);
	}
	//关闭数据库
	sqlite3_close(db);
}


void home_view(const gchar *userName){
	builder = gtk_builder_new();//指针分配空间

	//3.读取test.glade文件的信息，保存在builder中
	if (!gtk_builder_add_from_file(builder,"Home.glade", NULL)) {
		printf("connot load file!");
	}
	gtk_builder_add_from_file(builder,"Home.glade", NULL);//读取button.xml文件

	window_home = gtk_window_new(GTK_WINDOW_TOPLEVEL);  //创建窗口
	window_home = GTK_WIDGET(gtk_builder_get_object(builder, "home_window"));//获取window串口使用权
	btn_login = GTK_WIDGET(gtk_builder_get_object (builder, "btn_logout"));//注销按钮
	lb_user_name = GTK_WIDGET(gtk_builder_get_object (builder, "user_name"));//显示登陆的用户名
	gtk_label_set_text(GTK_LABEL(lb_user_name),userName);//显示用户名

	//新增页面
	//初始化表单
	add_kd_no = GTK_WIDGET(gtk_builder_get_object (builder, "kd_no"));//快递编号
	add_startAddress = GTK_WIDGET(gtk_builder_get_object (builder, "start_address"));//始发地址
	add_destAddress = GTK_WIDGET(gtk_builder_get_object (builder, "dest_address"));//目的地址
	add_user = GTK_WIDGET(gtk_builder_get_object (builder, "user"));//收货人
	add_userPhone = GTK_WIDGET(gtk_builder_get_object (builder, "user_phone"));//收货人电话
	add_btnSubmit = GTK_WIDGET(gtk_builder_get_object (builder, "btn_submit"));//提交按钮
	add_result = GTK_WIDGET(gtk_builder_get_object (builder, "add_result"));//结果文本框

	
	//搜索页面
	input_search = GTK_WIDGET(gtk_builder_get_object (builder, "input_search"));//搜索框
	btn_search = GTK_WIDGET(gtk_builder_get_object (builder, "btn_search"));//搜索按钮

	//显示Label控件
	ss_no = GTK_WIDGET(gtk_builder_get_object (builder, "ss_no"));
	ss_start = GTK_WIDGET(gtk_builder_get_object (builder, "ss_start"));
	ss_dest = GTK_WIDGET(gtk_builder_get_object (builder, "ss_dest"));
	ss_user = GTK_WIDGET(gtk_builder_get_object (builder, "ss_user"));
	ss_phone = GTK_WIDGET(gtk_builder_get_object (builder, "ss_phone"));
	ss_status = GTK_WIDGET(gtk_builder_get_object (builder, "ss_status"));
	ss_null = GTK_WIDGET(gtk_builder_get_object (builder, "ss_null"));

	order_list = GTK_WIDGET(gtk_builder_get_object(builder, "order_list"));//订单列表外层容器

	init_order_list();//初始化订单列表
	gtk_builder_connect_signals (builder, NULL);//连接响应事件
	g_object_unref (G_OBJECT (builder));  //释放xml内存空间
	gtk_widget_show (window_home);//显示窗体
	gtk_main();//事件响应函数
}

//新增快递信息
void on_btn_submit_clicked(){
	sqlite3 *db = NULL;
	int result,nRow,nColumn;
	char *errmsg;
	char **dbResult;
	char sql[150] = "";


	const gchar *kd_no = gtk_entry_get_text ((GtkEntry *)add_kd_no);
	const gchar *start_address = gtk_entry_get_text ((GtkEntry *)add_startAddress);
	const gchar *dest_address = gtk_entry_get_text ((GtkEntry *)add_destAddress);
	const gchar *user = gtk_entry_get_text ((GtkEntry *)add_user);
	const gchar *userPhone = gtk_entry_get_text ((GtkEntry *)add_userPhone);

	//判断用户是否输入
	if(strcmp(kd_no,(const gchar*)"") == 0){
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"请输入完整！");
		return;	
	}
	if(strcmp(start_address,(const gchar*)"") == 0){
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"请输入完整！");
		return;	
	}
	if(strcmp(dest_address,(const gchar*)"") == 0){
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"请输入完整！");
		return;	
	}
	if(strcmp(user,(const gchar*)"") == 0){
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"请输入完整！");
		return;	
	}
	if(strcmp(userPhone,(const gchar*)"") == 0){
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"请输入完整！");
		return;	
	}
		
	
	//打开数据库
	int ret = sqlite3_open("atlan.db",&db);
	if(ret != SQLITE_OK ){
		printf("数据库打开失败");
		return ;
	}
	//向数据库中插入记录
	sprintf(sql,"insert into goods(good_id,start_address,dest_address,user,user_phone) values('%s','%s','%s','%s','%s');",kd_no,start_address,dest_address,user,userPhone);
	result = sqlite3_get_table(db,sql,&dbResult,&nRow,&nColumn,&errmsg);
	if(result == SQLITE_OK)
	{
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"添加成功！");
		sqlite3_free_table(dbResult);
	}
	else
	{
		gtk_label_set_text(GTK_LABEL(add_result),(const gchar*)"添加失败！");
		printf("errmsg=%s\n",errmsg);
	}
	//关闭数据库
	sqlite3_close(db);
}


//注销按钮点击事件
void on_btn_logout_clicked(GtkButton *button,gpointer user_data)
{
	gtk_widget_destroy(window_home);
	gtk_main_quit();  //这里仅实现退出程序	
}


/**
*	登录页面代码
*/

//取消按钮点击事件
void btn_cancle_clicked_cb(GtkButton *button,gpointer user_data)
{
        gtk_main_quit();  //这里仅实现退出程序	
}

//登录按钮点击事件
void on_btn_login_clicked(GtkButton *button,gpointer user_data)
{	
	sqlite3 *db = NULL;
	int result,nRow,nColumn;
     	char *errmsg;
	char **dbResult;
	char sql[100] = "";

	const gchar *userName = gtk_entry_get_text ((GtkEntry *)txt_userName);
	const gchar *password = gtk_entry_get_text ((GtkEntry *)txt_password);
	gtk_label_set_text(GTK_LABEL(label_error),(const gchar*)"");//先初始化Label，清楚里面的内容
	//从数据库查询数据
	int ret = sqlite3_open("atlan.db",&db);
	if(ret != SQLITE_OK ){
		printf("数据库打开失败");
		return ;
	}
	sprintf(sql,"select * from users where username = '%s' and password = '%s';",userName,password);
	result = sqlite3_get_table(db,sql,&dbResult,&nRow,&nColumn,&errmsg);

	if(nRow > 0){
	        gtk_label_set_text(GTK_LABEL(label_error),(const gchar*)"登录成功！");
		gtk_widget_hide(window);
		home_view(userName);
		gtk_widget_destroy(window);
		sqlite3_free_table(dbResult);
	}
	else{
		gtk_label_set_text(GTK_LABEL(label_error),(const gchar*)"用户名或密码错误！");
	}
	//关闭数据库
	sqlite3_close(db);
}

//初始化登录窗口
void init_View(){
	builder = gtk_builder_new();//指针分配空间

	//读取test.glade文件的信息，保存在builder中
	if (!gtk_builder_add_from_file(builder,"login.glade", NULL)) {
		printf("布局文件读取失败!");
	}
	gtk_builder_add_from_file(builder,"login.glade", NULL);//读取button.xml文件

	//初始化窗口
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);  //创建窗口
	window = GTK_WIDGET(gtk_builder_get_object(builder, "window"));//获取window串口使用权
	
	//登录区域
	btn_login = GTK_WIDGET(gtk_builder_get_object (builder, "btn_login"));//获取btn_login控件使用权
	btn_cancle = GTK_WIDGET(gtk_builder_get_object (builder, "btn_cancle"));//获取btn_cancle控件使用权
	txt_userName = GTK_WIDGET(gtk_builder_get_object (builder, "txt_userName"));//用户名
	txt_password = GTK_WIDGET(gtk_builder_get_object (builder, "txt_password"));//密码
	label_error = GTK_WIDGET(gtk_builder_get_object (builder, "label_error"));//登录结果显示
	
	//窗口整合
	gtk_builder_connect_signals (builder, NULL);//连接响应事件
	g_object_unref (G_OBJECT (builder));  //释放xml内存空间
	gtk_widget_show (window);//显示窗体
	gtk_main();//事件响应函数
}

//主方法
int main(int argc,char *argv[]){
	gtk_init(&argc,&argv);
	init_View();


	return 0;
}

