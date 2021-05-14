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

#define WIDTH 800
#define HEIGHT 600
 
typedef struct FileEntry {
    SDL_Texture *name;
    SDL_Rect name_rect;

    SDL_Texture *file_size;
    SDL_Rect size_rect;

    SDL_Texture *permissions;
    SDL_Rect permissions_rect;

    int icon_index;
    SDL_Rect icon_rect;

    bool is_directory;

    std::string full_path;

} FileEntry;

typedef struct AppData {
    TTF_Font *font;

    SDL_Texture *icons[6];

    SDL_Texture *background;

    std::vector<FileEntry*> files;

    SDL_Rect background_rect;

    SDL_Point offset;
} AppData;
 
void initialize(SDL_Renderer *renderer, AppData *data_ptr);
void render(SDL_Renderer *renderer, AppData *data_ptr);
void quit(AppData *data_ptr);
void listDirectory(std::string dirname);
std::vector<FileEntry*> getDirectoryVector(std::string dirname, AppData *data_ptr, SDL_Renderer *renderer);

char* permissions(char *file);
 
int main(int argc, char **argv)
{
    AppData data;
    DIR *dr = opendir("..");
    struct dirent *pent = NULL;

    while(pent == readdir(dr)){
        if (pent == NULL){
            std::cout << "\nERROR! pent could not be initialised correctly";
            exit (3);
        }
        std::cout << pent->d_name << std::endl;
    }

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
                
                for(int i = 0; i < data.files.size(); i++)
                {
                    if (event.button.button == SDL_BUTTON_LEFT &&
                        event.button.x >= data.files[i]->name_rect.x &&
                        event.button.x <= data.files[i]->name_rect.x + data.files[i]->name_rect.w &&
                        event.button.y >= data.files[i]->name_rect.y &&
                        event.button.y <= data.files[i]->name_rect.y + data.files[i]->name_rect.h)
                    {
                        //change listDirectory to open clicked directory, or execute file
                        if(data.files[i]->icon_index == 0){
                            data.files = getDirectoryVector(data.files[i]->full_path, &data, renderer);
                        }
                        //execute file
                        else{
                            //fork, exec using xdg-open full_path
                        }
                    }else if (event.button.button == SDL_BUTTON_LEFT &&
                        event.button.x >= data.files[i]->icon_rect.x &&
                        event.button.x <= data.files[i]->icon_rect.x + data.files[i]->icon_rect.w &&
                        event.button.y >= data.files[i]->icon_rect.y &&
                        event.button.y <= data.files[i]->icon_rect.y + data.files[i]->icon_rect.h)
                    {
                        //change listDirectory to open clicked directory
                        if(data.files[i]->icon_index == 0){
                            data.files = getDirectoryVector(data.files[i]->full_path, &data, renderer);
                        }
                        //execute file
                        else{

                        }
                    }
                }
                
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
    if(data_ptr->font == NULL){
        printf("error loading font\n");
    }

    /*
    SDL_Surface *desktop_surf = IMG_Load("resrc/images/Desktop-icon.png");
    data_ptr->desktop = SDL_CreateTextureFromSurface(renderer, desktop_surf);
    SDL_FreeSurface(desktop_surf);
    data_ptr->desktop_rect.x = 10;
    data_ptr->desktop_rect.y = 40;
    data_ptr->desktop_rect.w = 165;
    data_ptr->desktop_rect.h = 200;

    SDL_Surface *documents_surf = IMG_Load("resrc/images/Documents-icon.png");
    data_ptr->documents = SDL_CreateTextureFromSurface(renderer, documents_surf);
    SDL_FreeSurface(documents_surf);
    data_ptr->documents_rect.x = 10;
    data_ptr->documents_rect.y = 80;
    data_ptr->documents_rect.w = 165;
    data_ptr->documents_rect.h = 200;

    SDL_Surface *downloads_surf = IMG_Load("resrc/images/Downloads-icon.png");
    data_ptr->downloads = SDL_CreateTextureFromSurface(renderer, downloads_surf);
    SDL_FreeSurface(downloads_surf);
    data_ptr->downloads_rect.x = 10;
    data_ptr->downloads_rect.y = 120;
    data_ptr->downloads_rect.w = 165;
    data_ptr->downloads_rect.h = 200;

    SDL_Surface *music_surf = IMG_Load("resrc/images/Music-icon.png");
    data_ptr->music = SDL_CreateTextureFromSurface(renderer, music_surf);
    SDL_FreeSurface(music_surf);
    data_ptr->music_rect.x = 10;
    data_ptr->music_rect.y = 160;
    data_ptr->music_rect.w = 165;
    data_ptr->music_rect.h = 200;

    SDL_Surface *pictures_surf = IMG_Load("resrc/images/Pictures-icon.png");
    data_ptr->pictures = SDL_CreateTextureFromSurface(renderer, pictures_surf);
    SDL_FreeSurface(pictures_surf);
    data_ptr->pictures_rect.x = 10;
    data_ptr->pictures_rect.y = 200;
    data_ptr->pictures_rect.w = 165;
    data_ptr->pictures_rect.h = 200;

    SDL_Surface *public_drive_surf = IMG_Load("resrc/images/Public-icon.png");
    data_ptr->public_drive = SDL_CreateTextureFromSurface(renderer, public_drive_surf);
    SDL_FreeSurface(public_drive_surf);
    data_ptr->public_drive_rect.x = 10;
    data_ptr->public_drive_rect.y = 240;
    data_ptr->public_drive_rect.w = 165;
    data_ptr->public_drive_rect.h = 200;

    SDL_Surface *templates_surf = IMG_Load("resrc/images/Templates-icon.png");
    data_ptr->templates = SDL_CreateTextureFromSurface(renderer, templates_surf);
    SDL_FreeSurface(templates_surf);
    data_ptr->templates_rect.x = 10;
    data_ptr->templates_rect.y = 280;
    data_ptr->templates_rect.w = 165;
    data_ptr->templates_rect.h = 200;

    SDL_Surface *videos_surf = IMG_Load("resrc/images/Videos-icon.png");
    data_ptr->videos = SDL_CreateTextureFromSurface(renderer, videos_surf);
    SDL_FreeSurface(videos_surf);
    data_ptr->videos_rect.x = 10;
    data_ptr->videos_rect.y = 320;
    data_ptr->videos_rect.w = 165;
    data_ptr->videos_rect.h = 200;

    SDL_Surface *snap_surf = IMG_Load("resrc/images/Snap-icon.png");
    data_ptr->snap = SDL_CreateTextureFromSurface(renderer, snap_surf);
    SDL_FreeSurface(snap_surf);
    data_ptr->snap_rect.x = 10;
    data_ptr->snap_rect.y = 360;
    data_ptr->snap_rect.w = 165;
    data_ptr->snap_rect.h = 200;
    */
    SDL_Color color = { 0, 0, 0 };//red, green, blue
    //SDL_Color background_color = { 255, 255, 255 };//red, green, blue
    SDL_Surface *background_surf = TTF_RenderText_Solid(data_ptr->font, "Icon | Name", color);

    data_ptr->background = SDL_CreateTextureFromSurface(renderer, background_surf);

    SDL_FreeSurface(background_surf);

    data_ptr->background_rect.x = 10;
    data_ptr->background_rect.y = 10;

    SDL_QueryTexture(data_ptr->background, NULL, NULL, &(data_ptr->background_rect.w), &(data_ptr->background_rect.h));

    char *home = getenv("HOME");
    printf("HOME: %s\n", home);

    data_ptr->files = getDirectoryVector(home, data_ptr, renderer);
}

// 1. Render only renders the first element in the vector
// 2. General process for populating the SDL window
// 3. Icons are not being printed at all
void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = { 0, 0, 0 };
    SDL_Rect rect;    rect.x = 200;    rect.y = 100;    rect.w = 165;    rect.h = 20;
    SDL_RenderCopy(renderer, data_ptr->background, NULL, &(data_ptr->background_rect));
    printf("%d files\n", data_ptr->files.size());
    for(int i = 0; i < data_ptr->files.size(); i++){
        if(data_ptr->files[i]->icon_index == 0){
            SDL_SetRenderDrawColor(renderer, 0, 0, 235, 255);
        }else {
            SDL_SetRenderDrawColor(renderer, 235, 0, 0, 255);
        }
        SDL_RenderFillRect(renderer, &(data_ptr->files[i]->icon_rect));
        SDL_RenderCopy(renderer, data_ptr->files[i]->name, NULL, &(data_ptr->files[i]->name_rect));//Only this should be in this loop
    }
 
    // show rendered frame
    SDL_RenderPresent(renderer);
}
 
void quit(AppData *data_ptr)
{
    SDL_DestroyTexture(data_ptr->background);
    for(int i = 0; i < data_ptr->files.size(); i++){
        SDL_DestroyTexture(data_ptr->files[i]->name);
    }
    TTF_CloseFont(data_ptr->font);
}
//Putting full path in dirname parameter moves it to new directory
std::vector<FileEntry*> getDirectoryVector(std::string dirname, AppData *data_ptr, SDL_Renderer *renderer)
{
    struct stat info;
    int err = stat(dirname.c_str(), &info);
    std::vector<FileEntry*> files;
    if (err == 0 && S_ISDIR(info.st_mode))
    {

        DIR* dir = opendir(dirname.c_str());
        struct dirent *entry;
        struct dirent *holder;
        struct stat file_info;//Not sure if this ever is filled in order to check type

        std::vector<std::string> directory_strings;

        while ((entry = readdir(dir)) != NULL) {

            //if(strcmp(entry.d_name[]))
            directory_strings.push_back(entry->d_name);
        }
        for(int i = 0; i < directory_strings.size(); i++){
            printf("%s\n", directory_strings[i].c_str());
        }
        printf("--------------------------------\n");
        closedir(dir);//at this point all the names are stored into the vector

        std::sort(directory_strings.begin(), directory_strings.end());//file names are sorted
        int a = 0;
        for(int i = 0; i < directory_strings.size(); i++ ){
            if(directory_strings[i][0] != '.' || directory_strings[i] == "..")
            {
                std::string full_path = dirname + "/" + directory_strings[i];
                
                //file_info = dirname.;
                printf("%s\n", full_path.c_str());

                err = stat(full_path.c_str(), &file_info);

                if(err){

                    fprintf(stderr, "uh oh, you shouldn't have gotten here \n");

                }else{
                    FileEntry* file = new FileEntry();
                        file->full_path = full_path;
                        //Create textures, rects, etc. separating with type
                        SDL_Color color = { 0, 0, 0 };
                        SDL_Surface *surface = TTF_RenderText_Solid(data_ptr->font, directory_strings[i].c_str(), color);
                        file->name = SDL_CreateTextureFromSurface(renderer, surface);
                        SDL_FreeSurface(surface);
                        //For first directory on page
                        file->name_rect.x = 60;
                        file->name_rect.y = 40 * a + 40;

                        SDL_QueryTexture(file->name, NULL, NULL, &(file->name_rect.w), &(file->name_rect.h));

                        file->icon_rect.x = 10;
                        file->icon_rect.y = file->name_rect.y;
                        file->icon_rect.h = 30;
                        file->icon_rect.w = 30;

                    if(S_ISDIR(file_info.st_mode) /*&& (directory_strings[i].find('.') != std::string::npos)*/)//if it is a directory-------------- ALWAYS FALSE (do we even need this?)
                    {
                        file->icon_index = 0;

                    }else{//checks if it's a different type, like permissions and extension
                        file->icon_index = 1;
                    }
                    files.push_back(file);

                    a++;
                }
            }
        }

    }else
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



