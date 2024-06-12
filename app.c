/* Todo 
    - Implement queue in timeline (how? maybe just use linked list and add a pointer to the last node)
    - add user profile
    - add post feature

    optional:
    - add repost feature (how? maybe just copy the post and add it as new post)
    
*/

#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LENGTH 255
#define MAX_CONTENT 512

typedef struct UserProfile {
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    char displayName[MAX_LENGTH];
    struct UserProfile *next;
} UserProfile;

typedef struct TreeNode {
    char username[MAX_LENGTH];
    char displayName[MAX_LENGTH];
    char content[MAX_CONTENT];
    int likes;
    struct TreeNode *next;
    struct TreeNode *child;
} TreeNode;

UserProfile *users = NULL;
TreeNode *posts = NULL;

/******************************************************************/
/******************************************************************/
/************************** User Profile *************************/
/******************************************************************/
/******************************************************************/

UserProfile *createUser(char *username, char *password, char *displayName) {
    UserProfile *newUser = (UserProfile *)malloc(sizeof(UserProfile));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->displayName, displayName);
    newUser->next = NULL;
    return newUser;
}

TreeNode *createPostNode(char *username, char *displayName, char *content) {
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    strcpy(newNode->username, username);
    strcpy(newNode->displayName, displayName);
    strcpy(newNode->content, content);
    newNode->likes = 0;
    newNode->next = NULL;
    newNode->child = NULL;
    return newNode;
}

void addUser(char *username, char *password, char *displayName) {
    UserProfile *temp = users;
    while (temp) {
        if (strcmp(temp->username, username) == 0) {
            printf("Username already exists!\n");
            return;
        }
        temp = temp->next;
    }

    UserProfile *newUser = createUser(username, password, displayName);
    newUser->next = users;
    users = newUser;
}

void loadUsers() {
    FILE *file = fopen("users.txt", "r");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    char username[MAX_LENGTH], password[MAX_LENGTH], displayName[MAX_LENGTH];
    while (fscanf(file, "%s %s %s", username, password, displayName) != EOF) {
        addUser(username, password, displayName);
    }

    fclose(file);
}

void saveUsers() {
    FILE *file = fopen("users.txt", "w");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    UserProfile *temp = users;
    while (temp) {
        fprintf(file, "%s %s %s\n", temp->username, temp->password, temp->displayName);
        temp = temp->next;
    }

    fclose(file);
}

int loginUser(char *username, char *password) {
    UserProfile *temp = users;
    while (temp) {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0) {
            printf("Login successful!\n");
            return 1;
        }
        temp = temp->next;
    }

    printf("Invalid username or password!\n");
    return 0;
}

void addPost(char *username, char *displayName, char *content) {
    TreeNode *newPost = createPostNode(username, displayName, content);
    newPost->next = posts;
    posts = newPost;
    savePosts();
}

void addPostWithoutSaving(char *username, char *displayName, char *content) {
    TreeNode *newPost = createPostNode(username, displayName, content);
    newPost->next = posts;
    posts = newPost;
}

void generatePosts(int numPosts) {
    char *usernames[] = {"dandywastaken", "rakhabuming", "rigiyoga", "wrath", "kastara05"};
    char *displayNames[] = {"Dandy", "Rakha", "Agoy", "Ausath", "Taqi"};
    char *contents[] = {"Hello, world!", "I love programming.", "It's a beautiful day.", "I'm learning C.", "This is a random post."};

    for (int i = 0; i < 5; i++) {
        char *username = usernames[i];
        char *displayName = displayNames[i];
        char *content = contents[i];
        addPost(username, displayName, content);
        // Randomly assign likes between 0 and 100
        posts->likes = rand() % 101;
    }
}

void savePosts() {
    FILE *file = fopen("posts.txt", "w");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    TreeNode *temp = posts;
    while (temp) {
        fprintf(file, "P|%s|%s|%s|%d\n", temp->username, temp->displayName, temp->content, temp->likes);
        TreeNode *reply = temp->child;
        while (reply) {
            fprintf(file, "R|%s|%s|%s|%d\n", reply->username, reply->displayName, reply->content, reply->likes);
            reply = reply->next;
        }
        temp = temp->next;
    }

    fclose(file);
}

void loadPosts() {
    FILE *file = fopen("posts.txt", "r");
    if (!file) {
        printf("Error opening file!\n");
        return;
    }

    char type, username[MAX_LENGTH], displayName[MAX_LENGTH], content[MAX_CONTENT];
    int likes;
    TreeNode *currentPost = NULL;

    while (fscanf(file, "%c|%[^|]|%[^|]|%[^|]|%d\n", &type, username, displayName, content, &likes) != EOF) {
        if (type == 'P') {
            TreeNode *newPost = createPostNode(username, displayName, content);
            newPost->likes = likes;
            newPost->next = posts;
            posts = newPost;
            currentPost = newPost;
        } else if (type == 'R' && currentPost != NULL) {
            TreeNode *newReply = createPostNode(username, displayName, content);
            newReply->likes = likes;
            if (!currentPost->child) {
                currentPost->child = newReply;
            } else {
                TreeNode *temp = currentPost->child;
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = newReply;
            }
        }
    }

    fclose(file);
}

void addReply(TreeNode *post, char *username, char *displayName, char *content) {
    TreeNode *newReply = createPostNode(username, displayName, content);
    if (!post->child) {
        post->child = newReply;
    } else {
        TreeNode *temp = post->child;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = newReply;
    }
    printf("Reply added successfully!\n");
    savePosts();
}

void likePost(TreeNode *post) {
    post->likes++;
    savePosts();
}

void displayPosts(TreeNode *post, int level) {
    while (post) {
        for (int i = 0; i < level; ++i) printf("  ");
        printf("%s\n", post->username);
        for (int i = 0; i < level; ++i) printf("  ");
        printf("%s\n", post->content);
        for (int i = 0; i < level; ++i) printf("  ");
        printf("<3 %d\n", post->likes);
        printf("\n");
        
        // Display replies in the specified format
        if (post->child) {
            TreeNode *reply = post->child;
            while (reply) {
                for (int i = 0; i < level + 1; ++i) printf("  ");
                printf("------> %s\n", reply->username);
                for (int i = 0; i < level + 1; ++i) printf("  ");
                printf("------- %s\n", reply->content);
                for (int i = 0; i < level + 1; ++i) printf("  ");
                printf("------- <3 %d\n", reply->likes);
                printf("\n");
                reply = reply->next;
            }
        }

        post = post->next;
    }
}

void displayTimeline(TreeNode *currentPost) {
    system("cls");
    printf("=== Timeline ===\n");
    displayPosts(currentPost, 0);
}

void handleUserDashboard(char *username, char *displayName) {
    int choice;
    char content[MAX_CONTENT];
    TreeNode *currentPost = posts;
    
    while (1) {
        displayTimeline(currentPost);

        printf("1. Reply to Post\n");
        printf("2. Like Post\n");
        printf("3. Next Post\n");
        printf("4. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        switch (choice) {
            case 1:
                if (currentPost) {
                    printf("Enter Reply Content: ");
                    fgets(content, MAX_CONTENT, stdin);
                    content[strcspn(content, "\n")] = 0; // Remove newline character
                    addReply(currentPost, username, displayName, content);
                } else {
                    printf("No post to reply to!\n");
                }
                break;
            case 2:
                if (currentPost) {
                    likePost(currentPost);
                } else {
                    printf("No post to like!\n");
                }
                break;
            case 3:
                if (currentPost) {
                    currentPost = currentPost->next;
                } else {
                    printf("No more posts!\n");
                }
                break;
            case 4:
                return;
            default:
                printf("Invalid choice! Please try again.\n");
        }
    }
}

int main() {
    int choice;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    char displayName[MAX_LENGTH];
    char content[MAX_CONTENT];
    int loggedIn = 0;

    loadUsers();
    loadPosts();

    while (1) {
        if (!loggedIn) {
            printf("==================================\n");
            printf("     Welcome to CLI Social Media\n");
            printf("==================================\n");
            printf("1. Sign Up\n");
            printf("2. Login\n");
            printf("3. Exit\n");
            printf("==================================\n");
            printf("Enter your choice: ");
            scanf("%d", &choice);
            getchar(); // Consume newline character

            switch (choice) {
                case 1:
                    printf("Enter Display Name: ");
                    fgets(displayName, MAX_LENGTH, stdin);
                    displayName[strcspn(displayName, "\n")] = 0;
                    printf("Enter Username: ");
                    fgets(username, MAX_LENGTH, stdin);
                    username[strcspn(username, "\n")] = 0;
                    printf("Enter Password: ");
                    fgets(password, MAX_LENGTH, stdin);
                    password[strcspn(password, "\n")] = 0;
                    addUser(username, password, displayName);
                    saveUsers();
                    break;
                case 2:
                    printf("Enter Username: ");
                    fgets(username, MAX_LENGTH, stdin);
                    username[strcspn(username, "\n")] = 0;
                    printf("Enter Password: ");
                    fgets(password, MAX_LENGTH, stdin);
                    password[strcspn(password, "\n")] = 0;
                    loggedIn = loginUser(username, password);
                    loggedIn = 1;
                    
                    break;
                case 3:
                    printf("Exiting...\n");
                    exit(0);
                default:
                    printf("Invalid choice! Please try again.\n");
            }
        } else {
            handleUserDashboard(username, displayName);
            loggedIn = 0;
        }
    }

    return 0;
}
