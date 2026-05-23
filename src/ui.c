#define _CRT_SECURE_NO_WARNINGS 
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ui.h"
#include "bst.h"

// 1. 입력 버퍼를 깨끗하게 비워주는 함수 (무한 루프 방지용)
void clearBuffer() {
    while (getchar() != '\n');
}

// 2. 숫자가 아닌 문자가 들어오면 걸러내는 완벽한 정수 입력 함수
int getValidInt(const char* prompt) {
    int value;
    while (1) {
        printf("%s", prompt);
        if (scanf("%d", &value) == 1) {
            clearBuffer();
            return value;
        }
        printf("[오류] 숫자로만 입력해주세요!\n");
        clearBuffer(); // 쓰레기값 비우기
    }
}

// 3. UI 메인 실행 함수
void runUI() {
    Bst tree;
    bst_init(&tree);

    int choice;
    while (1) {
        printf("\n=== 학생 성적 관리 시스템 ===\n");
        printf("1. 학생 성적 입력\n");
        printf("2. 학번으로 학생 검색\n");
        printf("3. 학생 정보 삭제\n");
        printf("0. 프로그램 종료\n");
        printf("=============================\n");

        choice = getValidInt("메뉴를 선택하세요: ");

        switch (choice) {
        case 1: {
            Student newStudent;
            printf("\n[학생 성적 입력]\n");
            newStudent.id = getValidInt("학번을 입력하세요: ");

            printf("이름을 입력하세요: ");
            fgets(newStudent.name, BST_NAME_MAX, stdin);
            newStudent.name[strcspn(newStudent.name, "\n")] = 0;

            // 성적 범위 예외 처리 추가 (0~100점)
            do {
                newStudent.grade = getValidInt("성적을 입력하세요 (0~100): ");
                if (newStudent.grade < 0 || newStudent.grade > 100) {
                    printf("[오류] 성적은 0에서 100 사이여야 합니다.\n");
                }
            } while (newStudent.grade < 0 || newStudent.grade > 100);

            // 백엔드 함수: 학생 데이터 삽입
            if (bst_insert(&tree, &newStudent) != NULL) {
                printf("-> [성공] %s 학생의 정보가 추가되었습니다.\n", newStudent.name);
            }
            else {
                printf("-> [실패] 이미 존재하는 학번이거나 추가에 실패했습니다.\n");
            }
            break;
        }
        case 2: {
            printf("\n[학생 검색]\n");
            int searchId = getValidInt("검색할 학번을 입력하세요: ");

            // 백엔드 함수: 학번으로 검색
            BstNode* result = bst_search(&tree, searchId);

            if (result != NULL) {
                printf("-> [검색 결과] 학번: %d | 이름: %s | 성적: %d\n",
                    result->data.id, result->data.name, result->data.grade);
            }
            else {
                printf("-> [알림] 해당 학번의 학생을 찾을 수 없습니다.\n");
            }
            break;
        }
        case 3: {
            printf("\n[학생 정보 삭제]\n");
            int deleteId = getValidInt("삭제할 학번을 입력하세요: ");

            // 백엔드 함수: 학번으로 삭제
            if (bst_delete(&tree, deleteId)) {
                printf("-> [성공] 학번 %d의 학생 정보가 삭제되었습니다.\n", deleteId);
            }
            else {
                printf("-> [실패] 해당 학번이 존재하지 않거나 삭제에 실패했습니다.\n");
            }
            break;
        }
        case 0:
            printf("\n프로그램을 종료합니다. 메모리를 정리 중...\n");
            bst_free(&tree);
            return;
        default:
            printf("-> [경고] 0~3 사이의 올바른 메뉴 번호를 입력해주세요.\n");
        } // switch 끝
    } // while 끝
