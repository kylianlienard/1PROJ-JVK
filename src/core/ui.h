// Tous les éléments d'interface répétés : boutons, titres, styles, inputs, animations si besoin

void showError(const char *text) {
    printf("%s\n", text);
}

short displayChoice(const char *question, const short numChoices, const char *choices[]) {
    short answer = 0;
    printf("+-- %s ---+\n", question);
    for (short i = 0; i < numChoices; i++) {
        printf("[%d] %s\n", i + 1, choices[i]);
    }
    while (answer < 1 || answer > numChoices) {
        printf("Choose an action: ");
        if (scanf("%hd", &answer) != 1) {
            while (getchar() != '\n');
            answer = 0;
        }
    }
    return answer;
}

int inputInt(const char *question, int maxInt) {
    int answer;
    while (1) {
        printf("%s: ", question);
        if (scanf("%d", &answer) != 1) {
            int c;
            while ((c = getchar()) != '\n' && c != EOF);
        } else if (answer >= 0 || answer <= maxInt) {
            break;
        }
    }
    return answer;
}

void inputStr(const char *question, char *inputAns, int maxChar, int uppercase) {
    printf("\n%s: ", question);
    scanf("%s", inputAns);
    inputAns[maxChar] = '\0';

    if (uppercase) {
        for (int i = 0; inputAns[i]; i++) {
            if (inputAns[i] >= 'a' && inputAns[i] <= 'z') {
                inputAns[i] -= 32;
            }
        }
    }
};
