#ifndef CORE_TASK_H
#define CORE_TASK_H
#include <core/base.h>

namespace core
{

class Task
{
public:
    typedef void (Component::*Handler) ();
    Task(Component* component, Handler handler);
    void start(uint32_t interval, int32_t loop = -1);
    void stop();
    bool running(){return (loop_!=0);}
private:
    void run_();
    Task* next_ = nullptr;
    uint32_t interval_;
    uint64_t nextTick_;
    int32_t loop_ = 0;
    Component *component_ = nullptr;
    Handler handler_;
    friend class Engine;
    friend class TickEvent;
};

}


#define _M_TASK(name)\
private:\
    core::Task name##Task_ = core::Task(this, (core::Task::Handler)&CLASS::name##Handler##_);\
    void name##Handler##_();\

#define _M_TASK_2(name,code)\
private:\
    core::Task name##Task_ = core::Task(this, (core::Task::Handler)&CLASS::name##Handler##_);\
    void name##Handler##_() code
#define M_TASK(...) _M_MACRO_2(__VA_ARGS__, _M_TASK_2, _M_TASK)(__VA_ARGS__)

#define M_TASK_HANDLER(cls,name) void cls::name##Handler##_()

#endif // TASK_H

/* Cơ chế hoạt động của Task
 * (1) Tạo ra 2 linklist gồm Task và activeTask
 * (2) Mỗi khi tạo task thì task mới được thêm vào Task
 * (3) Khi start 1 task nào thì task đó được gỡ ra khỏi Task và add vào activeTask
 * (4) Khi stop 1 task nào thì task đó được gỡ ra khỏi activeTask và add lại vào Task
 * (5) Hàm tick trong engine lợi dụng ngắt systick -> gọi 1ms 1 lần
 * (6) Mỗi khi Task xảy ra tick sẽ so sánh với tickCount_ với nextick_ của task gần nhất
 * (7) Khi thỏa mãn điều kiện -> post event -> gọi đến hàm execute trong engine
 * (8) Trong execute sẽ duyệt hết activeTask để tìm đến các task đã đến giờ thực thi
 * (9) Task nào đã đến giờ thực thi sẽ được gọi đến Handler tương ứng
 * (10) Đồng thời tìm nextTick nhỏ nhất để so sánh tiếp trong các chu kỳ sau như (6)
 */

