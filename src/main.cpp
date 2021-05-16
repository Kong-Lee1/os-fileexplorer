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
                            int pid = fork();
                            //child process
                            if (pid == 0)
                            {
                                std::string command = "xdg-open *";
                                execl("/usr/bin/xdg-open", "xdg-open", data.files[i]->full_path.c_str(), (char *)0);
                            }
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

    SDL_Surface *directory_surf = IMG_Load("resrc/Images/directory-icon.png");
    data_ptr->icons[0] = SDL_CreateTextureFromSurface(renderer, directory_surf);
    SDL_FreeSurface(directory_surf);

    SDL_Surface *executable_surf = IMG_Load("resrc/Images/executable-icon.png");
    data_ptr->icons[1] = SDL_CreateTextureFromSurface(renderer, executable_surf);
    SDL_FreeSurface(executable_surf);

    SDL_Surface *image_surf = IMG_Load("resrc/Images/image-icon.png");
    data_ptr->icons[2] = SDL_CreateTextureFromSurface(renderer, image_surf);
    SDL_FreeSurface(image_surf);

    SDL_Surface *video_surf = IMG_Load("resrc/Images/video-icon.png");
    data_ptr->icons[3] = SDL_CreateTextureFromSurface(renderer, video_surf);
    SDL_FreeSurface(video_surf);

    SDL_Surface *code_surf = IMG_Load("resrc/Images/code-icon.png");
    data_ptr->icons[4] = SDL_CreateTextureFromSurface(renderer, code_surf);
    SDL_FreeSurface(code_surf);

    SDL_Surface *other_surf = IMG_Load("resrc/Images/other-icon.jpg");
    data_ptr->icons[5] = SDL_CreateTextureFromSurface(renderer, other_surf);
    SDL_FreeSurface(other_surf);

    SDL_Color color = { 0, 0, 0 };
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

void render(SDL_Renderer *renderer, AppData *data_ptr)
{
    // erase renderer content
    SDL_SetRenderDrawColor(renderer, 235, 235, 235, 255);
    SDL_RenderClear(renderer);

    SDL_Color color = { 0, 0, 0 };
    SDL_Rect rect;    rect.x = 200;    rect.y = 100;    rect.w = 165;    rect.h = 20;
    SDL_RenderCopy(renderer, data_ptr->background, NULL, &(data_ptr->background_rect));

    for(int i = 0; i < data_ptr->files.size(); i++){
        if(data_ptr->files[i]->icon_index == 0){
            SDL_RenderCopy(renderer, data_ptr->icons[0], NULL, &(data_ptr->files[i]->icon_rect));
        }else if(data_ptr->files[i]->icon_index == 1){
            SDL_RenderCopy(renderer, data_ptr->icons[1], NULL, &(data_ptr->files[i]->icon_rect));
        }else if(data_ptr->files[i]->icon_index == 2){
            SDL_RenderCopy(renderer, data_ptr->icons[2], NULL, &(data_ptr->files[i]->icon_rect));
        }else if(data_ptr->files[i]->icon_index == 3){
            SDL_RenderCopy(renderer, data_ptr->icons[3], NULL, &(data_ptr->files[i]->icon_rect));
        }else if(data_ptr->files[i]->icon_index == 4){
            SDL_RenderCopy(renderer, data_ptr->icons[4], NULL, &(data_ptr->files[i]->icon_rect));
        }else if(data_ptr->files[i]->icon_index == 5){
            SDL_RenderCopy(renderer, data_ptr->icons[5], NULL, &(data_ptr->files[i]->icon_rect));
        }
        SDL_RenderCopy(renderer, data_ptr->files[i]->name, NULL, &(data_ptr->files[i]->name_rect));
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
        struct stat file_info;

        std::vector<std::string> directory_strings;

        while ((entry = readdir(dir)) != NULL) {

            //if(strcmp(entry.d_name[]))
            directory_strings.push_back(entry->d_name);
        }

        closedir(dir);//at this point all the names are stored into the vector

        std::sort(directory_strings.begin(), directory_strings.end());//file names are sorted
        int a = 0;
        for(int i = 0; i < directory_strings.size(); i++ ){
            if(directory_strings[i][0] != '.' || directory_strings[i] == "..")
            {
                std::string full_path = dirname + "/" + directory_strings[i];

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

                    if(S_ISDIR(file_info.st_mode) )//if it is a directory
                    {
                        file->icon_index = 0;

                    //checks if it's an executable
                    }else if((file_info.st_mode & S_IEXEC) != 0){
                        file->icon_index = 1;

                    //checks if it's an image
                    }else if(full_path.substr(full_path.find_last_of(".") + 1) == "jpg" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "jpeg" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "png" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "tif" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "tiff" ||
                                full_path.substr(full_path.find_last_of(".") + 1) == "gif"){
                        file->icon_index = 2;

                    //checks if it's a video
                    }else if(full_path.substr(full_path.find_last_of(".") + 1) == "mp4" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "mov" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "mkv" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "avi" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "webm"){
                        file->icon_index = 3;

                    //checks if it's a code file
                    }else if(full_path.substr(full_path.find_last_of(".") + 1) == "h" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "c" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "cpp" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "py" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "java" || 
                                full_path.substr(full_path.find_last_of(".") + 1) == "js"){
                        file->icon_index = 4;

                    }else{//checks if it's something else
                        file->icon_index = 5;

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



