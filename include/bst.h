#ifndef BST_H
#define BST_H

#include <stddef.h>
#include <stdbool.h>

// 학생 이름 최대 길이(널 종료 문자 포함)
#define BST_NAME_MAX 64

// 학생 데이터(학번이 기본 키)
typedef struct {
    int id;
    char name[BST_NAME_MAX];
    int grade;
} Student;

// BST 노드
typedef struct BstNode {
    Student data;
    struct BstNode *left;
    struct BstNode *right;
} BstNode;

// BST 루트 핸들
typedef struct {
    BstNode *root;
} Bst;

// 트리 초기화
void bst_init(Bst *tree);

// 학생 데이터 삽입(성공 시 해당 노드 포인터, 실패 시 NULL)
BstNode *bst_insert(Bst *tree, const Student *student);

// 학번 기준 탐색(없으면 NULL)
BstNode *bst_search(const Bst *tree, int id);

// 학번 기준 삭제(성공 true, 미존재·오류 false)
bool bst_delete(Bst *tree, int id);

// 전체 노드 메모리 해제 및 루트 초기화
void bst_free(Bst *tree);

#endif
