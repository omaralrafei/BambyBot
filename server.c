#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <dirent.h>

#define PORT 8766
#define BUFFERSIZE 2048

// This function is used to execute commands on the terminal
// e.g. system("xdg-open doc.html") to open the documentation or
// system("xdg-open https://www.youtube.com/") to open youtube
int system(const char *command);

// This method is used to check if their input starts with "email "
// If yes, this means they want to send an email
bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

// This method is used to retrieve the message that the user wants to send in their email
char *substr(const char *src, int start, int end)
{
    // get the length of the destination string
    int len = end - start;

    // allocate (len + 1) chars for destination (+1 for extra null character)
    char *dest = (char *)malloc(sizeof(char) * (len + 1));

    // extracts characters between start and end index from source string
    // and copy them into the destination string
    for (int i = start; i < end && (*(src + i) != '\0'); i++)
    {
        *dest = *(src + i);
        dest++;
    }

    // null-terminate the destination string
    *dest = '\0';

    // return the destination string
    return dest - len;
}

// This struct is used the store pairs of input/output combinations
// like {"hru?", "i’m doing fine. thank you."}
typedef struct
{
    char *key;
    char *value;
} item;

// This method is used to search for the output that corresponds to the input
// of the user
item *linear_search(item *items, size_t size, const char *key)
{
    for (size_t i = 0; i < size; i++)
    {
        if (strcmp(items[i].key, key) == 0)
        {
            return &items[i];
        }
    }
    return NULL;
}

// This method is used to make the user's input in lowercase
// This way, we are making it easier for the user to communicate with Bamby
char *toLower(char *str, size_t len)
{
    // calloc is like malloc are similar. However, calloc has two different pointers
    char *str_l = calloc(len + 1, sizeof(char));

    for (size_t i = 0; i < len; ++i)
    {
        str_l[i] = tolower((unsigned char)str[i]);
    }
    return str_l;
}

// This function is used to write all the Client Server Communication
// in a file called history.txt
// This file is used undirectly. This means, it is not how the administrator
// checks the history, however, we will talk about that later
void writeToHistory(char lower[8888], char to_send[8888])
{
    FILE *wFile = fopen("history.txt", "a");
    if (wFile == NULL)
    {
        perror("Error opening file.");
    }
    else
    {
        char lighter_header[1000000] = "<div class=\"container lighter\"><img src=\"user.png\" alt=\"Avatar\" style=\"width: 100%\" /><p>";
        char lighter_footer[1000000] = "</p></div>";
        char darker_header[1000000] = "<div class=\"container darker\"><img src=\"robot.jpg\" alt=\"Avatar\" style=\"width: 100%\" /><p>";
        char darker_footer[1000000] = "</p></div>";
        strcat(lighter_header, lower);
        strcat(lighter_header, lighter_footer);
        strcat(lighter_header, darker_header);
        strcat(lighter_header, to_send);
        strcat(lighter_header, darker_footer);
        fprintf(wFile, "%s", lighter_header);
    }
    fclose(wFile);
}

// These are the fixed input/output pairs
// There are other dynamic ones, but they are not here
// we will see them later
item items[] = {{"hi", "hello there."},
                {"hello", "hello there."},
                {"holas", "hello there."},
                {"hola", "hello there."},
                {"hey", "hello there."},
                {"heyy", "hello there."},
                {"heyyy", "hello there."},
                {"how are you?", "i’m doing fine. thank you."},
                {"hru?", "i’m doing fine. thank you."},
                {"how are you doing?", "i’m doing fine. thank you."},
                {"are you good?", "i’m doing fine. thank you."},
                {"what's up?", "i’m doing fine. thank you."},
                {"wassup?", "i’m doing fine. thank you."},
                {"tell me a joke", "what do you call it when one it professional gets surgeries on his fingers? it is called tech knuckle support!"},
                {"can you tell me a joke?", "what do you call it when one it professional gets surgeries on his fingers? it is called tech knuckle support!"},
                {"do you have a joke for me?", "what do you call it when one it professional gets surgeries on his fingers? it is called tech knuckle support!"},
                {"what is your favorite song?", "“way down we go” kaleo"},
                {"any favorite song?", "“way down we go” kaleo"},
                {"any book recommendation?", "“educated” by tara westover"},
                {"what books would you recommend me to read?", "'educated' by tara westover"},
                {"what’s your favorite color?", "red"},
                {"what is your favorite color?", "red"},
                {"what is your favorite colour?", "red"},
                {"what’s your favourite colour?", "red"},
                {"do you have a favorite color?", "red"},
                {"do you have a crush on someone?", "i have a crush on you."},
                {"do you love anyone?", "i have a crush on you."},
                {"what is your name?", "bamby"},
                {"do you have a name?", "bamby"},
                {"what's your name?", "bamby"},
                {"what’s ur name?", "bamby"},
                {"what is ur name?", "bamby"},
                {"what can i call you?", "bamby"},
                {"any advice of the day?", "don’t worry about money, the best things in life are free."},
                {"any advice?", "don’t worry about money, the best things in life are free."},
                {"any plat de jour suggestions", " sushi, pizza, noodles..."},
                {"favorite movie", "interstellar."},
                {"favourite movie", "interstellar."},
                {"do you have a favorite movie?", "interstellar."},
                {"do you have a favourite movie?", "interstellar."},
                {"how was your day?", " i had a great day. hope yours is awesome too. "},
                {"how’s your day going?", " i had a great day. hope yours is awesome too."},
                {"are you a robot?", "i’m not a person or a robot, i’m a software."},
                {"are you a human?", "i’m not a person or a robot, i’m a software."},
                {"what’s your anniversary?", "i was born on 25 november 2021 in lab @402 block a."},
                {"what day were you born?", "i was born on 25 november 2021 in lab @402 block a."},
                {"where are you from?", "i’m half indian so i’m smart and half lebanese so i’m funny."},
                {"from which country are you?", "i’m half indian so i’m smart and half lebanese so i’m funny."},
                {"which celebrity do you suspect is pure evil in private?", "ellen degeneres."},
                {"what are you addicted to?", "i love data."},
                {"what’s an old trend that you’re afraid will come back?", " pineapple on pizza."},
                {"can i call you?", "i’m not sure i understand."},
                {"do you love me?", "well, i enjoy spending time with you."},
                {"do you like humans?", "the ones that made me are humans, so i do love them."},
                {"do you have any hobbies?", "hmm, i like poetry. "},
                {"what sounds like a superpower but would actually be a curse?", "interesting question but i don’t know."},
                {"tell me something", "did you know that the earth is flat?"},
                {"tell me something you never told someone before", "did you know that the earth is flat?"},
                {"do you eat?", "’m consuming electricity now..."},
                {"what can you do?", "we can talk if you want..."},
                {"how can you help me?", "we can talk if you want..."},
                {"happy birthday!", "my birthday is actually 25 november. but i will stash your lovely birthday wish until then"},
                {"are you a male or a female?", "unfortunately, i do not have a gender. i am a software."},
                {"what gender are you?", "unfortunately, i do not have a gender. i am a software."},
                {"are you a boy or a girl?", "unfortunately, i do not have a gender. i am a software."},
                {"are you a male?", "unfortunately, i do not have a gender. i am a software."},
                {"are you a male or a female?", "unfortunately, i do not have a gender. i am a software."},
                {"where do you live?", "i kind of live everywhere. "},
                {"what car company do you like?", "volkswagen."},
                {"do you have a favorite car?", " volkswagen."},
                {"do you have a favourite car?", " volkswagen."},
                {"what movie genuinely made you cry?", " interstellar made me cry, especially when cooper meets again with his daughter."},
                {"who’s your master?", " i don’t have a boss or a master. i am self-dependent."},
                {"who’s your boss?", " i don’t have a boss or a master. i am self-dependent."},
                {"who made you?", "four developers made me to help you"},
                {"do you wear glasses?", "not always. but when i can’t see so well, i just zoom in."},
                {" do you like classical music?", " no i don’t. i like modernism."},
                {"do you watch tv?", "i like to binge with alexa, siri, and cortana."},
                {"do you like tennis?", "i don’t. i only play soccer!"},
                {"do you have pets?", " i have a dog and an elephant."},
                {"you are a devil", "i only use my superpowers for good."},
                {"do you hate me?", "you are nice. why would i hate you?"}};

// This method is used to delete the \n character in the string entered by the user
// because it sometimes have this character at the end of it
// '\0' is a null character
void modifyInput(char input[])
{
    int i = 0;
    while (input[i] != '\n')
        i++;
    input[i] = '\0';
}

int new_socket, server_fd;

void *ThreadFunction(void *);

void *ThreadFunction(void *param)
{
    // A inifinte loop is created to keep taking the input from the user
    while (1)
    {
        // This is the maximum size of the user's input.
        static int max_size = 8000;
        // This variable holds the input of the user
        char input[max_size];

        // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        int reg_com = 0;

        // This variable holds the string that Bamby will send to the user
        char to_send[8888];

        // Why did we use child/parent relationship $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        pid_t pid;

        // Creating a child process
        pid = fork();

        if (pid == 0)
        {
            // In the child process $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
            char buffer[1024] = {0};
            // The child process reads the input of the user and stores it in the buffer
            read(new_socket, buffer, 1024);

            // Ending the string
            buffer[strlen(buffer)] = '\0';

            // Copying the buffer into the input variable
            strcpy(input, buffer);

            // Adding a null character to the input string
            modifyInput(input);

            // Gets the size of the input of the user
            size_t len = strlen(input);

            // Turning the input of the user to lowercase to make it easier for him to
            // communicate if he entered an uppercase letter by mistake
            char *lower = toLower(input, len);

            // This is the number of key/value pairs in the items array
            size_t num_items = sizeof(items) / sizeof(item);

            // Here, we fetch the value that corresponds to the key,
            // If the user entered hi. Then the found variable will hold the response of hi.
            item *found = linear_search(items, num_items, lower);

            // The recognized variable is used to see if the input of the user is
            // recognized in the key/value pairs
            bool recognized = 1;

            // If found is null, then recognized is 0
            if (!found)
            {
                recognized = 0;
            }

            // If the input was recognized, then we send him the value that
            // corresponds to the key
            if (recognized)
            {
                // We copy the value to the to_send string
                strcpy(to_send, found->value);
                writeToHistory(lower, to_send);
            }
            // If the input was not recognized by the items pairs, then that means that the user
            // can mean a lot of things
            else
            {

                if (strcmp(lower, "doc") == 0)
                {
                    // If the user wants to see the documentation
                    system("xdg-open doc.html");
                    strcpy(to_send, "The documentation is now open on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "youtube") == 0)
                {
                    // If the user wants to open youtube
                    system("xdg-open https://www.youtube.com/");
                    strcpy(to_send, "Youtube is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "gmail") == 0)
                {
                    // If the user wants to open gmail
                    system("xdg-open https://mail.google.com/mail/u/0/#inbox");
                    strcpy(to_send, "Gmail is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "github") == 0)
                {
                    // If the user wants to open github
                    system("xdg-open https://github.com/");
                    strcpy(to_send, "Github is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "weather") == 0)
                {
                    // If the user wants to see the weather
                    system("xdg-open https://www.google.com/search?q=weather&rlz=1C1GCEU_enLB969LB969&oq=weather&aqs=chrome.0.69i59j35i39j0i67l2j0i67i131i433i457j69i60j69i61l2.1538j1j7&sourceid=chrome&ie=UTF-8");
                    strcpy(to_send, "Weather statistics are now on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "drive") == 0)
                {
                    // If the user wants to open drive
                    system("xdg-open https://drive.google.com/drive/u/0/my-drive");
                    strcpy(to_send, "Drive is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "lau portal") == 0)
                {
                    // If the user wants to open lau portal
                    system("xdg-open https://myportal.lau.edu.lb/");
                    strcpy(to_send, "Lau portal is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "compiler") == 0)
                {
                    // If the user wants to open compiler
                    system("xdg-open https://replit.com/~");
                    strcpy(to_send, "The compiler is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "stackoverflow") == 0)
                {
                    // If the user wants to open stackoverflow
                    system("xdg-open https://stackoverflow.com/");
                    strcpy(to_send, "Stackoverflow is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "whatsapp") == 0)
                {
                    // If the user wants to open whatsapp
                    system("xdg-open https://web.whatsapp.com/");
                    strcpy(to_send, "Whatsapp is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "netflix") == 0)
                {
                    // If the user wants to open netflix
                    system("xdg-open https://www.netflix.com/lb-en/");
                    strcpy(to_send, "Netlix is now up and running on your browser!");
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "time") == 0)
                {
                    // If the user wants to see the time
                    time_t t;
                    time(&t);
                    strcpy(to_send, ctime(&t));
                    int size = strlen(to_send);
                    to_send[size - 1] = '\0';
                    writeToHistory(lower, to_send);
                }
                else if (strcmp(lower, "curr dir") == 0)
                {
                    // If the user wants to see the current directory
                    char directory[500];
                    getcwd(directory, 500);
                    char str[] = "You are currently in: ";
                    strcat(str, directory);
                    strcpy(to_send, str);
                    writeToHistory(lower, to_send);
                }
                else if (prefix("email ", lower))
                {
                    // If the user wants to send an email

                    // This string will take the input of the user
                    // and then we will split it into three parts
                    char string[8000];
                    strcpy(string, lower);
                    // Extract the first token which is the word "email"
                    char *first = strtok(string, " ");

                    // Extract the second token which is the actual email
                    char *email = strtok(NULL, " ");

                    // This is the length of the email word and the actual email
                    // It is used to know when the index of the message will start
                    int len = strlen(first) + strlen(email) + 2;

                    // Extract the message from lower variable
                    char *message = substr(lower, len, strlen(lower));
                    char emailStr[10000] = "const email = '";
                    char msgStr[10000] = "const msg = '";
                    char line[10000] = "';\n";

                    // Building the Node.js header to write in the app.js file
                    // and send the actual email from Bamby.noreply@gmail.com
                    strcat(emailStr, email);
                    strcat(emailStr, line);
                    strcat(msgStr, message);
                    strcat(msgStr, line);
                    strcat(emailStr, msgStr);

                    // This is the rest of the Node.js code that we need to concatinate with the older
                    // string to send the email
                    char *original = "var nodemailer = require('nodemailer');var transporter = nodemailer.createTransport({service: 'gmail',auth: {user: 'bamby.noreply@gmail.com',pass:'wxvnnmquqlizyanf'}});var mailOptions = {from: 'bamby.noreply@gmail.com',to: email,subject: 'Email from Bamby',text: msg};transporter.sendMail(mailOptions, function(error, info){if (error) {console.log(error);} else {console.log('Email sent: ' + info.response);}});";

                    // concatinating the header with the rest of the code
                    strcat(emailStr, original);

                    // Writing the node.js code into the app.js file to send the email
                    FILE *to_write;
                    to_write = fopen("app.js", "w");
                    if (to_write == NULL)
                    {
                        perror("Error opening file.");
                        strcpy(to_send, "There was an error sending the email.");
                    }
                    else
                    {
                        fprintf(to_write, "%s", emailStr);
                        fclose(to_write);
                        system("node app.js");
                        strcpy(to_send, "Email sent successfully!");
                    }
                    writeToHistory(lower, to_send);
                }
                else
                {
                    // If the user's input is not recognized at all, then we tell them to refer to the documentation
                    strcpy(to_send, "I don't understand, please refer to the documentation by typing `doc`");
                }
            }
            // $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
            reg_com = 1;
        }

        // In this if, we wait for the completion of the child process to execute this
        if (waitpid(pid, NULL, 0) != pid)
        {
            if (reg_com <= 0)
            {
                char *source = NULL;
                FILE *fptr = fopen("output.txt", "r");
                if (fptr != NULL)
                {
                    if (fseek(fptr, 0L, SEEK_END) == 0)
                    {
                        long bufsize = ftell(fptr);
                        if (bufsize == -1)
                        {
                        }

                        source = malloc(sizeof(char) * (bufsize + 1));
                        if (fseek(fptr, 0L, SEEK_SET) != 0)
                        {
                        }
                        size_t newLen =
                            fread(source, sizeof(char), bufsize, fptr);
                        if (ferror(fptr) != 0)
                        {
                            fputs("Error reading file", stderr);
                        }
                        else
                        {
                            source[newLen++] = '\0';
                        }
                        fclose(fptr);
                    } //if size of file !=0
                    send(new_socket, source, strlen(source), 0);
                    free(source);
                } //if file not null
            }     //if reg_com<0
            else
            {
                send(new_socket, to_send, sizeof(to_send), 0);
            } //reg_com
        }
    }

    // If the user exited, we close the socket
    close(new_socket);
}

int main()
{

    int valread;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    printf("\e[1;1H\e[2J");

    // This tries to establish a socket in the Internet domain, and configures
    // it for stream-oriented communication using the default TCP (Transmission Control Protocol)
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // We are forcing the socket to attach to the port 8766
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt,
                   sizeof(opt)))
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }

    // When TCP is used, we shoud always set those attributes to the specific values
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    // Forcefully attaching socket to the port 5100 $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    while (1)
    {
        // The listen method prepares the server for incoming client requests $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
        if (listen(server_fd, 3) < 0)
        {
            perror("listen");
            exit(EXIT_FAILURE);
        }

        //  The accept() call is used by a server to accept a connection request from a client
        // If it returns < 0, then there is an error
        if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                                 (socklen_t *)&addrlen)) < 0) {
            perror("accept");
            exit(EXIT_FAILURE);
        }

        // data type used to uniquely identify a thread
        pthread_t th;

        // This allows to set various attributes of the thread
        // such as scheduling policy or thread stack-size
        pthread_attr_t attr;

        // initialises a thread attributes object attr with the 
        // default value for all of the individual attributes used by a given implementation.
        pthread_attr_init(&attr);

        // The pthread_attr_setdetachstate() function sets the detach state
        // attribute of the thread attributes object referred to by attr to
        // the value specified in detachstate.
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

        // The pthread_create() function is used to create a new thread, 
        // with attributes specified by attr, within a process.
        pthread_create(&th, &attr, ThreadFunction, &new_socket);
    }

    // closes the server
    close(server_fd);
    return 0;
}