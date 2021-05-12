#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <filesystem>
#include <unistd.h>
#include <cstdlib>

#include <vector>

#include <sys/stat.h>
#include <algorithm>
#include <string.h>
#include <string>

//using namespace std;
//namespace fs = std::experimental::filesystem;
 
#define WIDTH 800
#define HEIGHT 600
 
typedef struct AppData {
    TTF_Font *font;
    SDL_Texture *icon;
    SDL_Texture *background;
    SDL_Texture *directory;

    std::vector<SDL_Texture*> sdl_permissions;//will need helper function to grab permissions
    std::vector<SDL_Rect> sdl_directory_rect;
    std::vector<SDL_Texture*> sdl_names;//will need helper function to grab names
    std::vector<SDL_Texture*> sdl_file_size;//will need helper function to grab file_size
    std::vector<SDL_Texture*> sdl_file_type;//will need helper function to grab file_size


    SDL_Rect directory_rect;
    SDL_Rect icon_rect;
    SDL_Rect background_rect;
    bool icon_selected;
    bool directory_selected;

    bool desktop_selected;
    bool documents_selected;
    bool downloads_selected;
    bool music_selected;
    bool pictures_selected;
    bool public_drive_selected;
    bool templates_selected;
    bool videos_selected;
    bool snap_selected;

    SDL_Point offset;
    char *curdir;
} AppData;
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr, std::vector<std::string> directories);
//void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
std::vector<std::string> listDirectory(std::string dirname);
std::vector<std::string> get_permissions(std::vector<SDL_Texture> directory);
std::vector<std::string> get_names(std::vector<SDL_Texture> directory);
std::vector<std::string> get_files_sizes(std::vector<SDL_Texture> directory);
std::vector<std::string> get_file_types(std::vector<SDL_Texture> directory);

char* permissions(char *file);
 
int main(int argc, char **argv)
{
    AppData data;
    //How can we set directory to HOME and store it in AppData so it can be accessed/opened/closed in Main?
    DIR *dr = opendir("..");
    struct dirent *pent = NULL;
    std::vector<std::string> home_folders_and_files;
    std::vector<std::string> desktop_folders_and_files;



    while(pent == readdir(dr)){
        if (pent == NULL){
            std::cout << "\nERROR! pent could not be initialised correctly";
            exit (3);
        }
        std::cout << pent->d_name << std::endl;
    }
    char *home = getenv("HOME");
    data.curdir = home;
    //char *path = getenv("LS");
    printf("HOME: %s\n", home);

    //std::string homeToString = (std::string)home;

    listDirectory(home);
    home_folders_and_files = listDirectory(home);
    //desktop_folders_and_files = 
    //listDirectory((std::string)home + "/Desktop");

    //for(int i = 0; i < desktop_folders_and_files.size(); i++){

        //std::cout << "contents of desktop \n " << desktop_folders_and_files[i] << std::endl;
    //}


    
    
    //get_permissions(home);
    //permissions(home);

    //if(path != NULL){

        //printf("The current path is: %s \n", path);
    //}
    
    
 
    // initializing SDL as Video
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_PNG);
    TTF_Init();
 
    // create window and renderer
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    std::cout << "here 1" << std::endl;
 
    // initialize and perform rendering loop
    initialize(renderer, &data);
    std::cout << "here 1^^" << std::endl;
    //render(renderer, &data);
    render(renderer, &data, home_folders_and_files);
    std::cout << "here 1**" << std::endl;
    SDL_Event event;
    SDL_WaitEvent(&event);
    std::cout << "here 2" << std::endl;
    while (event.type != SDL_QUIT)
    {
        SDL_WaitEvent(&event);
        switch (event.type)
        {
            /*
            case SDL_MOUSEMOTION:
                if (data.penguin_selected)
                {
                    data.penguin_rect.x = event.motion.x - data.offset.x;
                    data.penguin_rect.y = event.motion.y - data.offset.y;
                }
                else if (data.phrase_selected)
                {
                    data.phrase_rect.x = event.motion.x - data.offset.x;
                    data.phrase_rect.y = event.motion.y - data.offset.y;
                }
                break;
            */
            case SDL_MOUSEBUTTONDOWN:
                //if the directory name is clicked
                if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.x >= data.directory_rect.x &&
                    event.button.x <= data.directory_rect.x + data.directory_rect.w &&
                    event.button.y >= data.directory_rect.y &&
                    event.button.y <= data.directory_rect.y + data.directory_rect.h)
                {
                    //How can we print all of the directories and make each of them their own 'surface' to be clicked on?
                    data.directory_selected = true;
                    //data.directory_rect.x = data.directory_rect.x + 20;
                    //data.directory_rect.y = data.directory_rect.y + 30;
                    //data.curdir = chdir(".");
                }
                //if the icon is clicked
                else if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.x >= data.icon_rect.x &&
                    event.button.x <= data.icon_rect.x + data.icon_rect.w &&
                    event.button.y >= data.icon_rect.y &&
                    event.button.y <= data.icon_rect.y + data.icon_rect.h)
                {
                    data.icon_selected = true;
                    data.offset.x = event.button.x - data.icon_rect.x;
                    data.offset.y = event.button.y - data.icon_rect.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                data.directory_selected = false;
                data.icon_selected = false;
                break;
        }
        std::cout << "here 3" << std::endl;
        //render(renderer, &data);
        render(renderer, &data, home_folders_and_files);
        std::cout << "here 4" << std::endl;
    }
 
    // clean up
    quit(&data);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
 
 
    return 0;
}
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr)
{
    data_ptr->font = TTF_OpenFont("resrc/OpenSans-Regular.ttf", 16);
    //std::cout << "i 1 \n" << std::endl;
    /*
    SDL_Surface *img_surf = IMG_Load("resrc/images/linux-penguin.png");
    data_ptr->penguin = SDL_CreateTextureFromSurface(renderer, img_surf);
    SDL_FreeSurface(img_surf);
    data_ptr->penguin_rect.x = 200;
    data_ptr->penguin_rect.y = 100;
    data_ptr->penguin_rect.w = 165;
    data_ptr->penguin_rect.h = 200;
    data_ptr->penguin_selected = false;
    */

    /*
    SDL_Surface *desktop_surf = IMG_Load("resrc/images/Desktop-icon.png");
    data_ptr->penguin = SDL_CreateTextureFromSurface(renderer, desktop_surf);
    SDL_FreeSurface(desktop_surf);
    data_ptr->desktop_rect.x = 200;
    data_ptr->desktop_rect.y = 100;
    data_ptr->desktop_rect.w = 165;
    data_ptr->desktop_rect.h = 200;
    data_ptr->desktop_selected = false;

    SDL_Surface *documents_surf = IMG_Load("resrc/images/Documents-icon.png");
    data_ptr->documents = SDL_CreateTextureFromSurface(renderer, documents_surf);
    SDL_FreeSurface(documents_surf);
    data_ptr->documents_rect.x = 200;
    data_ptr->documents_rect.y = 100;
    data_ptr->documents_rect.w = 165;
    data_ptr->documents_rect.h = 200;
    data_ptr->documents_selected = false;

    SDL_Surface *downloads_surf = IMG_Load("resrc/images/Downloads-icon.png");
    data_ptr->downloads = SDL_CreateTextureFromSurface(renderer, downloads_surf);
    SDL_FreeSurface(downloads_surf);
    data_ptr->downloads_rect.x = 200;
    data_ptr->downloads_rect.y = 100;
    data_ptr->downloads_rect.w = 165;
    data_ptr->downloads_rect.h = 200;
    data_ptr->downloads_selected = false;

    SDL_Surface *music_surf = IMG_Load("resrc/images/Music-icon.png");
    data_ptr->music = SDL_CreateTextureFromSurface(renderer, music_surf);
    SDL_FreeSurface(music_surf);
    data_ptr->music_rect.x = 200;
    data_ptr->music_rect.y = 100;
    data_ptr->music_rect.w = 165;
    data_ptr->music_rect.h = 200;
    data_ptr->music_selected = false;

    SDL_Surface *pictures_surf = IMG_Load("resrc/images/Pictures-icon.png");
    data_ptr->pictures = SDL_CreateTextureFromSurface(renderer, pictures_surf);
    SDL_FreeSurface(pictures_surf);
    data_ptr->pictures_rect.x = 200;
    data_ptr->pictures_rect.y = 100;
    data_ptr->pictures_rect.w = 165;
    data_ptr->pictures_rect.h = 200;
    data_ptr->pictures_selected = false;

    SDL_Surface *public_drive_surf = IMG_Load("resrc/images/Public-icon.png");
    data_ptr->public_drive = SDL_CreateTextureFromSurface(renderer, public_drive_surf);
    SDL_FreeSurface(public_drive_surf);
    data_ptr->public_drive_rect.x = 200;
    data_ptr->public_drive_rect.y = 100;
    data_ptr->public_drive_rect.w = 165;
    data_ptr->public_drive_rect.h = 200;
    data_ptr->public_drive_selected = false;

    SDL_Surface *templates_surf = IMG_Load("resrc/images/Templates-icon.png");
    data_ptr->templates = SDL_CreateTextureFromSurface(renderer, templates_surf);
    SDL_FreeSurface(templates_surf);
    data_ptr->templates_rect.x = 200;
    data_ptr->templates_rect.y = 100;
    data_ptr->templates_rect.w = 165;
    data_ptr->templates_rect.h = 200;
    data_ptr->templates_selected = false;

    SDL_Surface *videos_surf = IMG_Load("resrc/images/Videos-icon.png");
    data_ptr->videos = SDL_CreateTextureFromSurface(renderer, videos_surf);
    SDL_FreeSurface(videos_surf);
    data_ptr->videos_rect.x = 200;
    data_ptr->videos_rect.y = 100;
    data_ptr->videos_rect.w = 165;
    data_ptr->videos_rect.h = 200;
    data_ptr->videos_selected = false;

    SDL_Surface *snap_surf = IMG_Load("resrc/images/Snap-icon.png");
    data_ptr->snap = SDL_CreateTextureFromSurface(renderer, snap_surf);
    SDL_FreeSurface(img_surf);
    data_ptr->snap_rect.x = 200;
    data_ptr->snap_rect.y = 100;
    data_ptr->snap_rect.w = 165;
    data_ptr->snap_rect.h = 200;
    data_ptr->snap_selected = false;
    */
    SDL_Color color = { 0, 0, 0 };//red, green, blue
    //SDL_Color background_color = { 255, 255, 255 };//red, green, blue
    SDL_Surface *background_surf = TTF_RenderText_Solid(data_ptr->font, "Name                |Size     |Type      |User     |Permissions", color);
    //std::cout << "i 2 \n" << std::endl;
    //What should be initialized besides the background?
    //std::cout << "i 3 \n" << std::endl;

    data_ptr->background = SDL_CreateTextureFromSurface(renderer, background_surf);
    //std::cout << "i 4 \n" << std::endl;
    std::cout << "i 5 \n" << std::endl;

    SDL_FreeSurface(background_surf);
    //std::cout << "i 6 \n" << std::endl;
    //std::cout << "i 7 \n" << std::endl;

    data_ptr->background_rect.x = 10;
    data_ptr->background_rect.y = 10;

    //std::cout << "i 8 \n" << std::endl;
    SDL_QueryTexture(data_ptr->background, NULL, NULL, &(data_ptr->background_rect.w), &(data_ptr->background_rect.h));
    //std::cout << "i 9 \n" << std::endl;
    //std::cout << "i 10 \n" << std::endl;
    data_ptr->directory_selected = false;
}
 
void render(SDL_Renderer *renderer, AppData *data_ptr, std::vector<std::string> directories)
//void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    std::cout << "i 1 \n" << std::endl;
    SDL_RenderClear(renderer);
    std::cout << "i 2 \n" << std::endl;
     
    //std::vector<std::string> directories = listDirectory(data_ptr->curdir);
    std::cout << "i 3 \n" << std::endl;

    SDL_Color color = { 0, 0, 0 };
    SDL_RenderCopy(renderer, data_ptr->background, NULL, &(data_ptr->background_rect));
    std::cout << "i 4 \n" << std::endl;
    
    for(int i = 0; i < directories.size(); i++){

        std::cout << "i 5 \n" << std::endl;
        SDL_Surface *directory_surf = TTF_RenderText_Solid(data_ptr->font, directories[i].c_str(), color);
        std::cout << "i 6 \n" << std::endl;
        data_ptr->sdl_names[i] = SDL_CreateTextureFromSurface(renderer, directory_surf);
        std::cout << "i 7 \n" << std::endl;
        SDL_FreeSurface(directory_surf);
        std::cout << "i 8 \n" << std::endl;

        if(i == 0){
            data_ptr->sdl_directory_rect[i].x = 10;
            data_ptr->sdl_directory_rect[i].y = 40;
            std::cout << "i 9 \n" << std::endl;
        }else{
            data_ptr->sdl_directory_rect[i].x = data_ptr->sdl_directory_rect[i-1].x;
            data_ptr->sdl_directory_rect[i].x = data_ptr->sdl_directory_rect[i-1].y + 40;
            std::cout << "i 10 \n" << std::endl;
        }
        SDL_QueryTexture(data_ptr->directory, NULL, NULL, &(data_ptr->sdl_directory_rect[i].w), &(data_ptr->sdl_directory_rect[i].h));
        std::cout << "i 11 \n" << std::endl;
        SDL_RenderCopy(renderer, data_ptr->sdl_names[i], NULL, &(data_ptr->sdl_directory_rect[i]));
        std::cout << "i 12 \n" << std::endl;
    }
 
    // show rendered frame
    SDL_RenderPresent(renderer);
    std::cout << "i 13 \n" << std::endl;
}
 
void quit(AppData *data_ptr)
{
    SDL_DestroyTexture(data_ptr->icon);
    SDL_DestroyTexture(data_ptr->background);
    SDL_DestroyTexture(data_ptr->directory);
    TTF_CloseFont(data_ptr->font);
}
//How can we implement this function?
std::vector<std::string> listDirectory(std::string dirname)
{
    struct stat info;
    int err = stat(dirname.c_str(), &info);
    std::vector<std::string> files;
    if (err == 0 && S_ISDIR(info.st_mode))
    {
        
        DIR* dir = opendir(dirname.c_str());
        // TODO: modify to be able to print all entries in directory in alphabetical order
        //       in addition to file name, also print file size (or 'directory' if entry is a folder)
        //       Example output:
        //         ls.cpp (693 bytes
        //         my_file.txt (62 bytes)
        //         OS Files (directory)
        struct dirent *entry;
        struct dirent *holder;

        while ((entry = readdir(dir)) != NULL) {

            //if(strcmp(entry.d_name[]))
            files.push_back(entry->d_name);
        }
        closedir(dir);
        //std::cout << "files is \n" << files[1];
    
        std::sort(files.begin(), files.end());
        int i;
        struct stat file_info;
        std::cout << "here";
        std::cout << "file size is " << files.size() << "\n";
        for(i =0; i < (files.begin() - files.end()); i++ ){
            //printf("%s testing \n", files[i]);

            if(files[i].find('.') == std::string::npos){

                files.push_back(files[i]);
                //std::cout << files[i] << std::endl;

            }
            std::cout << files[i] << std::endl;
            //std::cout << "file size is " << files.size() << "\n";
            //names without a period is a folder, so grab theses.
            //std::cout << "entry is \n" << entry->d_name;
            //printf("%s\n, (%d bytes)\n", entry->d_name, entry->d_reclen);
            //std::cout << entry->d_name << entry->d_reclen << std::endl;
        }
        //printf("%s, (%d bytes)\n", entry->d_name, entry->d_reclen);
        //std::cout << entry->d_name << entry->d_reclen << std::endl;
        
        
        return files;
        
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }

    return files;
}

//to grab file's permissions
/*std::vector<std::string> get_permissions(std::vector<SDL_Texture> directory){
    
    struct stat info;
    int err = stat(dirname.c_str(), &info);
    std::vector<std::string> files;
    std::vector<SDL_Texture*> files_permissions;
    
    
    if (err == 0 && S_ISDIR(info.st_mode))
    {
        
        DIR* dir = opendir(dirname.c_str());
        // TODO: modify to be able to print all entries in directory in alphabetical order
        //       in addition to file name, also print file size (or 'directory' if entry is a folder)
        //       Example output:
        //         ls.cpp (693 bytes
        //         my_file.txt (62 bytes)
        //         OS Files (directory)
        struct dirent *entry;
        struct dirent *holder;

        while ((entry = readdir(dir)) != NULL) {

            //if(strcmp(entry.d_name[]))
            files_permissions.push_back((SDL_Texture*)entry->d_name);
        }
        closedir(dir);
        //std::cout << "files is \n" << files[1];
    
        std::sort(files_permissions.begin(), files_permissions.end());
        int i;
        struct stat file_info;
        //std::cout << "file size is \n" << files.size();
        for(i =0; i < files_permissions.size(); i++ ){
            //printf("%s testing \n", files[i]);

            if(files[i].find('.') == std::string::npos){
                

                
                
                files_permissions.push_back((SDL_Texture*)(permissions((char *) files_permissions[i])));
                std::cout << files_permissions[i] << std::endl;

            }
            //std::cout << files[i] << std::endl;
            //names without a period is a folder, so grab theses.
            //std::cout << "entry is \n" << entry->d_name;
            //printf("%s\n, (%d bytes)\n", entry->d_name, entry->d_reclen);
            //std::cout << entry->d_name << entry->d_reclen << std::endl;
        }
        //printf("%s, (%d bytes)\n", entry->d_name, entry->d_reclen);
        //std::cout << entry->d_name << entry->d_reclen << std::endl;
        return files_permissions;
        
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }



    return files_permissions;

}
*/
char* permissions(char *file){
    struct stat st;
    char *modeval = (char *)malloc(sizeof(char) * 9 + 1);
    if(stat(file, &st) == 0){
        mode_t perm = st.st_mode;
        modeval[0] = (perm & S_IRUSR) ? 'r' : '-';
        modeval[1] = (perm & S_IWUSR) ? 'w' : '-';
        modeval[2] = (perm & S_IXUSR) ? 'x' : '-';
        modeval[3] = (perm & S_IRGRP) ? 'r' : '-';
        modeval[4] = (perm & S_IWGRP) ? 'w' : '-';
        modeval[5] = (perm & S_IXGRP) ? 'x' : '-';
        modeval[6] = (perm & S_IROTH) ? 'r' : '-';
        modeval[7] = (perm & S_IWOTH) ? 'w' : '-';
        modeval[8] = (perm & S_IXOTH) ? 'x' : '-';
        modeval[9] = '\0';

        std::cout << modeval << std::endl;
        return modeval;     
    }
    else{
        return strerror(errno);
    }   
}

//to grab folders/files names
std::vector<std::string> get_names(std::vector<SDL_Texture> directories);

//to grab file's sizes
std::vector<std::string> get_files_sizes(std::vector<SDL_Texture> directories);

//to grab file's types
std::vector<std::string> get_file_types(std::vector<SDL_Texture> directories);

std::vector<std::string> get_permissions(std::vector<SDL_Texture> directories);




