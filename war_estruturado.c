#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Struct que representa um território
typedef struct {
    char nome[30];
    char cor[10];
    int tropas;
} Territorio;

// Função para cadastrar territórios...
void cadastrarTerritorios(Territorio* mapa, int n) {
    for (int i = 0; i < n; i++) {
        printf("\n--- Cadastro do territorio %d de %d ---\n", i + 1, n);
        printf("Nome: ");
        // fgets para permitir espaços; removemos o \n final
        fgets(mapa[i].nome, sizeof(mapa[i].nome), stdin);
        size_t len = strlen(mapa[i].nome);
        if (len > 0 && mapa[i].nome[len-1] == '\n') mapa[i].nome[len-1] = '\0';

        printf("Cor (ex: Vermelho, Azul): ");
        fgets(mapa[i].cor, sizeof(mapa[i].cor), stdin);
        len = strlen(mapa[i].cor);
        if (len > 0 && mapa[i].cor[len-1] == '\n') mapa[i].cor[len-1] = '\0';

        printf("Quantidade de tropas (inteiro positivo): ");
        while (scanf("%d", &mapa[i].tropas) != 1 || mapa[i].tropas < 0) {
            while (getchar() != '\n');
            printf("Valor invalido. Digite um numero inteiro nao-negativo: ");
        }
        while (getchar() != '\n'); // limpar buffer
    }
}

// Função para exibir todos os territórios
void exibirMapa(Territorio* mapa, int n) {
    printf("\n====== Mapa de Territorios ======\n");
    for (int i = 0; i < n; i++) {
        printf("[%d] Nome: %s | Cor: %s | Tropas: %d\n", i, mapa[i].nome, mapa[i].cor, mapa[i].tropas);
    }
    printf("================================\n");
}

// Valida se indices estao no alcance
int indiceValido(int idx, int n) {
    return idx >= 0 && idx < n;
}

// Função que simula um ataque entre dois territórios
// Todos os acessos/modificacoes são feitos por ponteiros conforme solicitado
void atacar(Territorio* atacante, Territorio* defensor) {
    if (atacante == NULL || defensor == NULL) return;

    // Simula uma rolagem de dado para cada lado (1 a 6)
    int dadoAtacante = rand() % 6 + 1;
    int dadoDefensor = rand() % 6 + 1;

    printf("\nRolagem de dados: Atacante(%s) -> %d  |  Defensor(%s) -> %d\n",
           atacante->nome, dadoAtacante, defensor->nome, dadoDefensor);

    if (dadoAtacante > dadoDefensor) {
        // Atacante vence: defensor muda de dono para a cor do atacante
        // e recebe metade (inteiro) das tropas do atacante.
        int tropasTransferidas = atacante->tropas / 2; // truncamento int
        if (tropasTransferidas < 1) tropasTransferidas = 1; // garantir ocupacao minima

        printf("Atacante vence! Transferindo %d tropas e alterando controle.\n", tropasTransferidas);

        // Atualiza defensor
        strncpy(defensor->cor, atacante->cor, sizeof(defensor->cor) - 1);
        defensor->cor[sizeof(defensor->cor)-1] = '\0';
        defensor->tropas = tropasTransferidas;

        // Atualiza atacante: perde as tropas transferidas
        atacante->tropas -= tropasTransferidas;
        if (atacante->tropas < 0) atacante->tropas = 0;
    } else {
        // Atacante perde: perde 1 tropa (se tiver)
        printf("Defensor resiste! Atacante perde 1 tropa.\n");
        if (atacante->tropas > 0) atacante->tropas -= 1;
    }
}

// Libera memoria alocada para o mapa (simples wrapper)
void liberarMemoria(Territorio* mapa) {
    free(mapa);
}

int main() {
    srand((unsigned int)time(NULL)); // inicializa gerador aleatorio

    int n;
    printf("Quantos territorios deseja cadastrar? ");
    while (scanf("%d", &n) != 1 || n <= 0) {
        while (getchar() != '\n');
        printf("Valor invalido. Digite um inteiro positivo: ");
    }
    while (getchar() != '\n'); // limpar buffer

    // Aloca dinamicamente o vetor de territorios usando calloc
    Territorio* mapa = (Territorio*) calloc(n, sizeof(Territorio));
    if (mapa == NULL) {
        fprintf(stderr, "Erro ao alocar memoria.\n");
        return 1;
    }

    // Cadastro dos territorios
    cadastrarTerritorios(mapa, n);

    // Loop principal: permitir que o jogador simule ataques
    int opcao = 0;
    while (1) {
        exibirMapa(mapa, n);
        printf("\nEscolha uma opcao:\n");
        printf("1 - Realizar um ataque\n");
        printf("2 - Sair\n");
        printf("Opcao: ");
        if (scanf("%d", &opcao) != 1) {
            while (getchar() != '\n');
            printf("Entrada invalida. Tente novamente.\n");
            continue;
        }
        while (getchar() != '\n'); // limpar buffer

        if (opcao == 2) break;
        if (opcao != 1) {
            printf("Opcao desconhecida. Tente novamente.\n");
            continue;
        }

        int idxAtacante, idxDefensor;
        printf("Digite o indice do territorio atacante: ");
        if (scanf("%d", &idxAtacante) != 1) {
            while (getchar() != '\n');
            printf("Entrada invalida.\n");
            continue;
        }
        printf("Digite o indice do territorio defensor: ");
        if (scanf("%d", &idxDefensor) != 1) {
            while (getchar() != '\n');
            printf("Entrada invalida.\n");
            continue;
        }
        while (getchar() != '\n');

        if (!indiceValido(idxAtacante, n) || !indiceValido(idxDefensor, n)) {
            printf("Indices fora do intervalo. Tente novamente.\n");
            continue;
        }
        if (idxAtacante == idxDefensor) {
            printf("Um territorio nao pode atacar a si mesmo.\n");
            continue;
        }

        // Validar que os territorios nao pertencem ao mesmo dono (mesma cor)
        if (strcmp(mapa[idxAtacante].cor, mapa[idxDefensor].cor) == 0) {
            printf("Atencao: nao e permitido atacar um territorio da propria cor.\n");
            continue;
        }

        // Realiza o ataque via ponteiros
        atacar(&mapa[idxAtacante], &mapa[idxDefensor]);

        // Exibe os dados atualizados
        exibirMapa(mapa, n);

        // Opcional: se quiser encerrar automaticamente quando restar apenas 1 cor, poderia implementar aqui.
    }

    // Libera memoria e encerra
    liberarMemoria(mapa);
    printf("Memoria liberada. Programa encerrado.\n");
    return 0;
}

