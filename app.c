#include <conio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <windows.h>

#define MAX_LENGTH 255
#define MAX_CONTENT 512

typedef struct UserProfile
{
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    char displayName[MAX_LENGTH];
    struct UserProfile *next;
} UserProfile;

typedef struct TreeNode
{
    char username[MAX_LENGTH];
    char displayName[MAX_LENGTH];
    char content[MAX_CONTENT];
    int likes;
    int reposts;
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

void savePosts();
void displayTimeline(TreeNode *currentPost);

UserProfile *createUser(char *username, char *password, char *displayName)
{
    UserProfile *newUser = (UserProfile *)malloc(sizeof(UserProfile));
    strcpy(newUser->username, username);
    strcpy(newUser->password, password);
    strcpy(newUser->displayName, displayName);
    newUser->next = NULL;
    return newUser;
}

TreeNode *createPostNode(char *username, char *displayName, char *content)
{
    TreeNode *newNode = (TreeNode *)malloc(sizeof(TreeNode));
    strcpy(newNode->username, username);
    strcpy(newNode->displayName, displayName);
    strcpy(newNode->content, content);
    newNode->likes = 0;
    newNode->reposts = 0;
    newNode->next = NULL;
    newNode->child = NULL;
    return newNode;
}

void addUser(char *username, char *password, char *displayName)
{
    UserProfile *temp = users;
    while (temp)
    {
        if (strcmp(temp->username, username) == 0)
        {
            printf("Username already exists!\n");
            return;
        }
        temp = temp->next;
    }

    UserProfile *newUser = createUser(username, password, displayName);
    newUser->next = users;
    users = newUser;
}

void loadUsers()
{
    FILE *file = fopen("users.txt", "r");
    if (!file)
    {
        printf("Error opening file!\n");
        return;
    }

    char username[MAX_LENGTH], password[MAX_LENGTH], displayName[MAX_LENGTH];
    while (fscanf(file, "%[^|]|%[^|]|%[^\n]\n", username, password, displayName) != EOF)
    {
        addUser(username, password, displayName);
    }

    fclose(file);
}

void saveUsers()
{
    FILE *file = fopen("users.txt", "w");
    if (!file)
    {
        printf("Error opening file!\n");
        return;
    }

    UserProfile *temp = users;
    while (temp)
    {
        fprintf(file, "%s|%s|%s\n", temp->username, temp->password, temp->displayName);
        temp = temp->next;
    }

    fclose(file);
}

UserProfile *loginUser(char *username, char *password)
{
    UserProfile *temp = users;
    while (temp)
    {
        if (strcmp(temp->username, username) == 0 && strcmp(temp->password, password) == 0)
        {
            printf("Login successful!\n");
            return temp;
        }
        temp = temp->next;
    }

    printf("Invalid username or password!\n");
    return NULL;
}

TreeNode *addPost(char *username, char *displayName, char *content)
{
    TreeNode *newPost = createPostNode(username, displayName, content);
    newPost->next = posts;
    posts = newPost;
    savePosts();
    displayTimeline(posts);
    return newPost;
}

void addPostWithoutSaving(char *username, char *displayName, char *content)
{
    TreeNode *newPost = createPostNode(username, displayName, content);
    newPost->next = posts;
    posts = newPost;
}

void savePosts()
{
    FILE *file = fopen("posts.txt", "w");
    if (!file)
    {
        printf("Error opening file!\n");
        return;
    }

    TreeNode *temp = posts;
    while (temp)
    {
        fprintf(file, "P|%s|%s|%s|%d|%d\n", temp->username, temp->displayName, temp->content, temp->likes, temp->reposts); // Add reposts
        TreeNode *reply = temp->child;
        while (reply)
        {
            fprintf(file, "R|%s|%s|%s|%d|%d\n", reply->username, reply->displayName, reply->content, reply->likes, reply->reposts); // Add reposts
            reply = reply->next;
        }
        temp = temp->next;
    }

    fclose(file);
}

void loadPosts()
{
    FILE *file = fopen("posts.txt", "r");
    if (!file)
    {
        printf("Error opening file!\n");
        return;
    }

    char type, username[MAX_LENGTH], displayName[MAX_LENGTH], content[MAX_CONTENT];
    int likes, reposts;
    TreeNode *currentPost = NULL;

    // Temporary stack to hold posts in reverse order
    TreeNode *reverseStack = NULL;

    while (fscanf(file, "%c|%[^|]|%[^|]|%[^|]|%d|%d\n", &type, username, displayName, content, &likes, &reposts) != EOF)
    {
        if (type == 'P')
        {
            TreeNode *newPost = createPostNode(username, displayName, content);
            newPost->likes = likes;
            newPost->reposts = reposts;

            // Push the new post onto the reverse stack
            newPost->next = reverseStack;
            reverseStack = newPost;

            currentPost = newPost;
        }
        else if (type == 'R' && currentPost != NULL)
        {
            TreeNode *newReply = createPostNode(username, displayName, content);
            newReply->likes = likes;
            newReply->reposts = reposts;
            if (!currentPost->child)
            {
                currentPost->child = newReply;
            }
            else
            {
                TreeNode *temp = currentPost->child;
                while (temp->next)
                {
                    temp = temp->next;
                }
                temp->next = newReply;
            }
        }
    }

    // Pop posts from the reverse stack to maintain original order
    while (reverseStack)
    {
        TreeNode *temp = reverseStack;
        reverseStack = reverseStack->next;
        temp->next = posts;
        posts = temp;
    }

    fclose(file);
}

void addReply(TreeNode *post, char *username, char *displayName, char *content)
{
    TreeNode *newReply = createPostNode(username, displayName, content);
    if (!post->child)
    {
        post->child = newReply;
    }
    else
    {
        TreeNode *temp = post->child;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = newReply;
    }
    printf("Reply added successfully!\n");
    savePosts();
}

void likePost(TreeNode *post)
{
    post->likes++;
    savePosts();
}

TreeNode *repost(TreeNode *originalPost, char *username, char *displayName)
{
    originalPost->reposts++; // Increment repost count of the original post
    char repostContent[MAX_CONTENT + MAX_LENGTH];
    snprintf(repostContent, sizeof(repostContent), "Reposted by %s\n%s\n%s\n<3 %d | Reposts: %d", displayName, originalPost->username, originalPost->content, originalPost->likes, originalPost->reposts);
    TreeNode *repostNode = addPost(username, displayName, repostContent);
    repostNode->reposts = originalPost->reposts; // Sync the repost count
    return repostNode;
}

void displayPosts(TreeNode *post, int level)
{
    while (post)
    {
        for (int i = 0; i < level; ++i)
            printf("  ");
        if (strstr(post->content, "Reposted by") == post->content)
        {
            printf("%s\n", post->content); // Display the repost content directly
        }
        else
        {
            printf("%s\n", post->username);
            for (int i = 0; i < level; ++i)
                printf("  ");
            printf("%s\n", post->content);
            for (int i = 0; i < level; ++i)
                printf("  ");
            printf("<3 %d | Reposts %d\n", post->likes, post->reposts); // Display repost count
        }
        printf("\n");

        if (post->child)
        {
            TreeNode *reply = post->child;
            int replyIndex = 1;
            while (reply)
            {
                for (int i = 0; i < level + 1; ++i)
                    printf("  ");
                printf("[%d] ", replyIndex);
                if (reply->child)
                {
                    printf("|------> %s\n", reply->username);
                    for (int i = 0; i < level + 1; ++i)
                        printf("  ");
                    printf("    |------ %s\n", reply->content);
                    for (int i = 0; i < level + 1; ++i)
                        printf("  ");
                    printf("    |------ <3 %d | Reposts %d\n", reply->likes, reply->reposts);
                }
                else
                {
                    printf("------> %s\n", reply->username);
                    for (int i = 0; i < level + 1; ++i)
                        printf("  ");
                    printf("------- %s\n", reply->content);
                    for (int i = 0; i < level + 1; ++i)
                        printf("  ");
                    printf("------- <3 %d | Reposts %d\n", reply->likes, reply->reposts);
                }
                printf("\n");

                // If the reply has its own replies, display them
                if (reply->child)
                {
                    displayPosts(reply->child, level + 2);
                }

                reply = reply->next;
                replyIndex++;
            }
        }

        post = post->next;
    }
}

void displayTimeline(TreeNode *currentPost)
{
    system("cls");
    printf("=== Timeline ===\n");
    displayPosts(currentPost, 0);
}

void displayUserPosts(char *username)
{
    system("cls");
    TreeNode *currentPost = posts;
    printf("=== %s's Posts ===\n", username);

    while (currentPost)
    {
        if (strcmp(currentPost->username, username) == 0)
        {
            printf("%s\n", currentPost->content);
            if (strstr(currentPost->content, "Reposted by") != currentPost->content)
            {
                printf("<3 %d\n", currentPost->likes);
            }
            printf("\n");
        }
        currentPost = currentPost->next;
    }

    // Option to go back to the timeline
    printf("Enter any key to go back to the timeline...");
    getch(); // Wait for a key press
    displayTimeline(posts);
}

void handleReplyDashboard(TreeNode *reply, char *username, char *displayName)
{
    int choice;
    char content[MAX_CONTENT];

    while (1)
    {
        system("cls");
        printf("\n=== Reply ===\n");
        printf("%s\n", reply->username);
        printf("%s\n", reply->content);
        printf("<3 %d\n", reply->likes);
        printf("\n1. Add Nested Reply\n");
        printf("2. Like Reply\n");
        printf("3. Back to Post\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        switch (choice)
        {
        case 1:
            printf("Enter Reply Content: ");
            fgets(content, MAX_CONTENT, stdin);
            content[strcspn(content, "\n")] = 0; // Remove newline character
            addReply(reply, username, displayName, content);
            break;
        case 2:
            likePost(reply);
            break;
        case 3:
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

void handleUserDashboard(char *username, char *displayName)
{
    int choice;
    char content[MAX_CONTENT];
    TreeNode *currentPost = posts;
    printf("Welcome, %s!\n", displayName);
    Sleep(1500);
    displayTimeline(currentPost);

    while (1)
    {
        printf("1. Add New Post\n");
        printf("2. Reply to Post\n");
        printf("3. Like Post\n");
        printf("4. Next Post\n");
        printf("5. Step into Post\n");
        printf("6. Repost\n");
        printf("7. View My Posts\n");
        printf("8. Logout\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Consume newline character

        switch (choice)
        {
        case 1:
            printf("Enter Post Content: ");
            fgets(content, MAX_CONTENT, stdin);
            content[strcspn(content, "\n")] = 0; // Remove newline character
            currentPost = addPost(username, displayName, content);
            break;
        case 2:
            if (currentPost)
            {
                printf("Enter Reply Content: ");
                fgets(content, MAX_CONTENT, stdin);
                content[strcspn(content, "\n")] = 0; // Remove newline character
                addReply(currentPost, username, displayName, content);
                displayTimeline(currentPost);
            }
            else
            {
                printf("No post to reply to!\n");
            }
            break;
        case 3:
            if (currentPost)
            {
                likePost(currentPost);
                displayTimeline(currentPost);
            }
            else
            {
                printf("No post to like!\n");
            }
            break;
        case 4:
            if (currentPost)
            {
                currentPost = currentPost->next;
                displayTimeline(currentPost);
            }
            else
            {
                printf("No more posts!\n");
            }
            break;
        case 5:
            if (currentPost)
            {
                int replyIndex;
                printf("Enter reply index to step into: ");
                scanf("%d", &replyIndex);
                getchar(); // Consume newline character

                TreeNode *selectedReply = currentPost->child;
                int currentIndex = 1;
                while (selectedReply && currentIndex < replyIndex)
                {
                    selectedReply = selectedReply->next;
                    currentIndex++;
                }

                if (selectedReply)
                {
                    handleReplyDashboard(selectedReply, username, displayName);
                    displayTimeline(posts); // Refresh timeline after returning from reply dashboard
                }
                else
                {
                    printf("Invalid reply index!\n");
                }
            }
            else
            {
                printf("No post to step into!\n");
            }
            break;
        case 6:
            if (currentPost)
            {
                currentPost = repost(currentPost, username, displayName);
            }
            else
            {
                printf("No post to repost!\n");
            }
            break;
        case 7:
            displayUserPosts(username);
            break;
        case 8:
            return;
        default:
            printf("Invalid choice! Please try again.\n");
        }
    }
}

int main()
{
    int choice;
    char username[MAX_LENGTH];
    char password[MAX_LENGTH];
    char displayName[MAX_LENGTH];
    char content[MAX_CONTENT];
    UserProfile *loggedIn = NULL;

    loadUsers();
    loadPosts();

    while (1)
    {
        if (!loggedIn)
        {
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

            switch (choice)
            {
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
                if (loggedIn)
                {
                    strcpy(displayName, loggedIn->displayName);
                }
                break;
            case 3:
                printf("Exiting...\n");
                exit(0);
            default:
                printf("Invalid choice! Please try again.\n");
            }
        }
        else
        {
            handleUserDashboard(username, displayName);
            loggedIn = NULL;
        }
    }

    return 0;
}
