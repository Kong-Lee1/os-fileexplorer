#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <filesystem>
#include <unistd.h>

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
    SDL_Rect directory_rect;
    SDL_Rect icon_rect;
    SDL_Rect background_rect;
    bool icon_selected;
    bool directory_selected;
    SDL_Point offset;
    char *curdir;
} AppData;
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
void listDirectory(std::string dirname);
 
int main(int argc, char **argv)
{
    AppData data;
    char *home = getenv("HOME");
    data.curdir = home;
    //char *path = getenv("LS");
    printf("HOME: %s\n", home);

    std::string homeToString = (std::string)home;

    listDirectory(home);

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
 
    // initialize and perform rendering loop
    initialize(renderer, &data);
    render(renderer, &data);
    SDL_Event event;
    SDL_WaitEvent(&event);
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
                    //expand directory
                    data.directory_selected = true;
                    data.directory_rect.x = data.directory_rect.x + 20;
                    data.directory_rect.y = data.directory_rect.y + 30;
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
 
        render(renderer, &data);
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
    SDL_Color color = { 0, 0, 0 };
    SDL_Surface *background_surf = TTF_RenderText_Solid(data_ptr->font, "Name                |Size     |Type      |User     |Permissions", color);
    SDL_Surface *directory_surf = TTF_RenderText_Solid(data_ptr->font, data_ptr->curdir, color);
    data_ptr->background = SDL_CreateTextureFromSurface(renderer, background_surf);
    data_ptr->directory = SDL_CreateTextureFromSurface(renderer, directory_surf);
    SDL_FreeSurface(background_surf);
    SDL_FreeSurface(directory_surf);
    data_ptr->background_rect.x = 10;
    data_ptr->background_rect.y = 10;
    data_ptr->directory_rect.x = 10;
    data_ptr->directory_rect.y = 40;
    SDL_QueryTexture(data_ptr->background, NULL, NULL, &(data_ptr->background_rect.w), &(data_ptr->background_rect.h));
    SDL_QueryTexture(data_ptr->directory, NULL, NULL, &(data_ptr->directory_rect.w), &(data_ptr->directory_rect.h));
    //data_ptr->directory_selected = false;
}
 
void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
     
    // TODO: draw!
    //SDL_RenderCopy(renderer, data_ptr->penguin, NULL, &(data_ptr->penguin_rect));
    if(data_ptr->directory_selected){
        SDL_RenderCopy(renderer, data_ptr->directory, NULL, &(data_ptr->directory_rect));
    }
    SDL_RenderCopy(renderer, data_ptr->directory, NULL, &(data_ptr->directory_rect));
    SDL_RenderCopy(renderer, data_ptr->background, NULL, &(data_ptr->background_rect));
 
    // show rendered frame
    SDL_RenderPresent(renderer);
}
 
void quit(AppData *data_ptr)
{
    SDL_DestroyTexture(data_ptr->icon);
    SDL_DestroyTexture(data_ptr->background);
    TTF_CloseFont(data_ptr->font);
}

void listDirectory(std::string dirname)
{
    struct stat info;
    int err = stat(dirname.c_str(), &info);
    if (err == 0 && S_ISDIR(info.st_mode))
    {
        std::vector<std::string> files;
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
        //std::cout << "file size is \n" << files.size();
        for(i =0; i < files.size() - 1; i++ ){
            //printf("%s testing \n", files[i]);
            std::cout << "files is \n" << files[i];
            //names without a period is a folder, so grab theses.
            //std::cout << "entry is \n" << entry->d_name;
            //printf("%s\n, (%d bytes)\n", entry->d_name, entry->d_reclen);
        }
        //printf("%s, (%d bytes)\n", entry->d_name, entry->d_reclen);
        
    }
    else
    {
        fprintf(stderr, "Error: directory '%s' not found\n", dirname.c_str());
    }
}