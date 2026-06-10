# Student-bst-system

> C + BST + Raylib로 구현한 GUI 기반 학생 관리 프로그램

---

# Overview

이 프로젝트는 이진 탐색 트리를 활용하여
학생 데이터를 효율적으로 관리하고,
Raylib 기반 GUI를 통해 직관적인 사용자 인터페이스를 제공하는 프로그램입니다.

기존의 콘솔 프로그램과 달리
데스크톱 앱 형태 UI를 구현한 것이 특징입니다.


# Features

* GUI Interface

  * Raylib 기반 메뉴 UI
  * 직관적인 사용자 경험 제공

* Smart Search

  * 학번 검색 → BST 탐색 (빠름)
  * 이름 검색 → 전체 순회 + 문자열 비교

* Auto Sorting

  * 중위 순회(In-order traversal)
  * 항상 학번 기준 오름차순 유지

* Modular Structure

  * UI / 로직 분리
  * 유지보수 및 확장성 고려

# Project Structure

Student-bst-system/
├── src/
│   ├── main.c     # entry point
│   ├── ui.c       # GUI (Raylib)
│   ├── bst.c      # BST logic
│
├── include/
│   ├── ui.h
│   ├── bst.h


# Build & Run

# 1. Compile

gcc main.c ui.c bst.c -o program.exe

# 2. Run

./program.exe

# Tech Stack

* C
* Raylib
* Binary Search Tree (BST)


# Key Idea

* BST를 활용한 빠른 데이터 처리
* GUI를 통한 사용자 친화적 인터페이스
* 중위 순회를 통한 자동 정렬

# Summary

BST + GUI + 모듈화 구조를 결합한 C 기반 응용 프로그램
