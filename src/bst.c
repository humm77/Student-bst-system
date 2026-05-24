#include "bst.h"

#include <stdlib.h>
#include <string.h>

// 학생 데이터를 목적지 구조체에 안전하게 복사
static void student_copy(Student *dest, const Student *src)
{
    if (dest == NULL || src == NULL) {
        return;
    }
    dest->id = src->id;
    dest->grade = src->grade;
    strncpy(dest->name, src->name, BST_NAME_MAX - 1);
    dest->name[BST_NAME_MAX - 1] = '\0';
}

// 새 노드 동적 할당 및 학생 데이터 설정
static BstNode *bst_node_create(const Student *student)
{
    BstNode *node;

    if (student == NULL) {
        return NULL;
    }

    node = (BstNode *)malloc(sizeof(BstNode));
    if (node == NULL) {
        return NULL;
    }

    student_copy(&node->data, student);
    node->left = NULL;
    node->right = NULL;
    return node;
}

// 재귀 삽입(중복 학번이면 inserted를 0으로 유지)
static BstNode *bst_insert_rec(BstNode *root, const Student *student, int *inserted)
{
    if (root == NULL) {
        BstNode *node = bst_node_create(student);
        if (node != NULL) {
            *inserted = 1;
        }
        return node;
    }

    if (student->id < root->data.id) {
        root->left = bst_insert_rec(root->left, student, inserted);
    } else if (student->id > root->data.id) {
        root->right = bst_insert_rec(root->right, student, inserted);
    }

    return root;
}

// 재귀 탐색
static BstNode *bst_search_rec(BstNode *root, int id)
{
    if (root == NULL) {
        return NULL;
    }
    if (id < root->data.id) {
        return bst_search_rec(root->left, id);
    }
    if (id > root->data.id) {
        return bst_search_rec(root->right, id);
    }
    return root;
}

// 서브트리에서 최소 학번 노드 탐색
static BstNode *bst_find_min(BstNode *root)
{
    if (root == NULL) {
        return NULL;
    }
    while (root->left != NULL) {
        root = root->left;
    }
    return root;
}

// 재귀 삭제(deleted 플래그로 삭제 여부 기록)
static BstNode *bst_delete_rec(BstNode *root, int id, int *deleted)
{
    BstNode *successor;
    BstNode *temp;
    int dummy;

    if (root == NULL) {
        return NULL;
    }

    if (id < root->data.id) {
        root->left = bst_delete_rec(root->left, id, deleted);
        return root;
    }
    if (id > root->data.id) {
        root->right = bst_delete_rec(root->right, id, deleted);
        return root;
    }

    *deleted = 1;

    if (root->left == NULL) {
        temp = root->right;
        free(root);
        return temp;
    }
    if (root->right == NULL) {
        temp = root->left;
        free(root);
        return temp;
    }

    successor = bst_find_min(root->right);
    if (successor == NULL) {
        return root;
    }

    student_copy(&root->data, &successor->data);
    dummy = 0;
    root->right = bst_delete_rec(root->right, successor->data.id, &dummy);
    return root;
}

// 후위 순회로 전체 노드 해제
static void bst_free_rec(BstNode *root)
{
    if (root == NULL) {
        return;
    }
    bst_free_rec(root->left);
    bst_free_rec(root->right);
    free(root);
}

void bst_init(Bst *tree)
{
    if (tree == NULL) {
        return;
    }
    tree->root = NULL;
}

BstNode *bst_insert(Bst *tree, const Student *student)
{
    int inserted;

    if (tree == NULL || student == NULL) {
        return NULL;
    }

    inserted = 0;
    tree->root = bst_insert_rec(tree->root, student, &inserted);
    if (inserted == 0) {
        return NULL;
    }

    return bst_search(tree, student->id);
}

BstNode *bst_search(const Bst *tree, int id)
{
    if (tree == NULL) {
        return NULL;
    }
    return bst_search_rec(tree->root, id);
}

bool bst_delete(Bst *tree, int id)
{
    int deleted;

    if (tree == NULL) {
        return false;
    }

    deleted = 0;
    tree->root = bst_delete_rec(tree->root, id, &deleted);
    return deleted != 0;
}

void bst_free(Bst *tree)
{
    if (tree == NULL) {
        return;
    }
    bst_free_rec(tree->root);
    tree->root = NULL;
}

// 중위 순회 재귀
static void bst_inorder_rec(BstNode *root, BstVisitor visitor, void *user_data)
{
    if (root == NULL || visitor == NULL) {
        return;
    }
    bst_inorder_rec(root->left, visitor, user_data);
    visitor(&root->data, user_data);
    bst_inorder_rec(root->right, visitor, user_data);
}

void bst_inorder(const Bst *tree, BstVisitor visitor, void *user_data)
{
    if (tree == NULL || visitor == NULL) {
        return;
    }
    bst_inorder_rec(tree->root, visitor, user_data);
}
