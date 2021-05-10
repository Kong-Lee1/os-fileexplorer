#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include <dirent.h>
#include <filesystem>

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
    SDL_Texture *penguin;
    SDL_Texture *phrase;
    SDL_Texture *new_phrase;
    SDL_Rect new_phrase_rect;
    SDL_Rect penguin_rect;
    SDL_Rect phrase_rect;
    bool penguin_selected;
    bool phrase_selected;
    SDL_Point offset;
} AppData;
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
void listDirectory(std::string dirname);
 
int main(int argc, char **argv)
{
    char *home = getenv("HOME");
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
    AppData data;
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
                    event.button.x >= data.phrase_rect.x &&
                    event.button.x <= data.phrase_rect.x + data.phrase_rect.w &&
                    event.button.y >= data.phrase_rect.y &&
                    event.button.y <= data.phrase_rect.y + data.phrase_rect.h)
                {
                    //expand directory
                    data.phrase_selected = true;
                    data.new_phrase_rect.x = data.phrase_rect.x + 20;
                    data.new_phrase_rect.y = data.phrase_rect.y + 30;

                }
                //if the icon is clicked
                else if (event.button.button == SDL_BUTTON_LEFT &&
                    event.button.x >= data.penguin_rect.x &&
                    event.button.x <= data.penguin_rect.x + data.penguin_rect.w &&
                    event.button.y >= data.penguin_rect.y &&
                    event.button.y <= data.penguin_rect.y + data.penguin_rect.h)
                {
                    data.penguin_selected = true;
                    data.offset.x = event.button.x - data.penguin_rect.x;
                    data.offset.y = event.button.y - data.penguin_rect.y;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                data.phrase_selected = false;
                data.penguin_selected = false;
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
    SDL_Surface *phrase_surf = TTF_RenderText_Solid(data_ptr->font, "Name                |Size     |Type      |User     |Permissions", color);
    SDL_Surface *new_phrase_surf = TTF_RenderText_Solid(data_ptr->font, "DIRECTORY", color);
    data_ptr->phrase = SDL_CreateTextureFromSurface(renderer, phrase_surf);
    data_ptr->new_phrase = SDL_CreateTextureFromSurface(renderer, new_phrase_surf);
    SDL_FreeSurface(phrase_surf);
    SDL_FreeSurface(new_phrase_surf);
    data_ptr->phrase_rect.x = 10;
    data_ptr->phrase_rect.y = 10;
    SDL_QueryTexture(data_ptr->phrase, NULL, NULL, &(data_ptr->phrase_rect.w), &(data_ptr->phrase_rect.h));
    SDL_QueryTexture(data_ptr->new_phrase, NULL, NULL, &(data_ptr->new_phrase_rect.w), &(data_ptr->new_phrase_rect.h));
    data_ptr->phrase_selected = false;
}
 
void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);
     
    // TODO: draw!
    //SDL_RenderCopy(renderer, data_ptr->penguin, NULL, &(data_ptr->penguin_rect));
    if(data_ptr->phrase_selected){
        SDL_RenderCopy(renderer, data_ptr->new_phrase, NULL, &(data_ptr->new_phrase_rect));
    }
    SDL_RenderCopy(renderer, data_ptr->phrase, NULL, &(data_ptr->phrase_rect));
 
    // show rendered frame
    SDL_RenderPresent(renderer);
}
 
void quit(AppData *data_ptr)
{
    //SDL_DestroyTexture(data_ptr->penguin);
    SDL_DestroyTexture(data_ptr->phrase);
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