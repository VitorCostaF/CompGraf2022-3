RA: 11079215
A presente atividade consiste no jogo nonogram ou logic pix. Para maiores detalhes consulte https://pt.wikipedia.org/wiki/Logic_Pix. A ideia do jogo basicamente é formar a figura descrita pelos números ao lado e em cima da imagem. Os número representam os quadrados e quantos devem ser seguidos. Por exemplo se temos um 5 em uma coluna significa que teremos 5 quadrados pretos seguidos. Se tivermos um 1 e 2 teremos um quadrado preto com um ou mais brancos seguidos de 2 pretos. 

O Jogo se inicia com a chamada da função startGame(), que inicia o m_board, variável que representa o tabuleiro, com brancos e coloca o estado do jogo Play.
Na onPaintUI temos a criação do menu com o item game, que possui a opção de restart do nível, e o outro item levels, que possui os levels liberados par o jogador escolher. Um level é liberado se o jogador passou por ele.

Logo depois temos a montagem do tabuleiro. O tamanho do tabuleiro é definido pela vetor levelsSize. Esse vetor guarda o tamanho do tabuleiro de cada level. Na primeira linha e coluna não temos botões mas sim labels dos números de preenchimento. Ao clicar em Mark podemos marcar clicar os botões para marca-los com a cor preta para formar o desenho indicado pelos números e no botão Unmark para pintar os botões de branco, para formar as separações. 

No tabuleiro m_board fica marcado com 0, 1 ou ' ' (espaço) para indicar se o campo foi marcado ou não. Mais abaixo temos os botões de reset, mark e unmark. Sempre que um botão é pressionado é chamada a função para checar o fim do jogo. Basicamente uma função lambda é usada para conferir se a string formada pela concatenação de m_board é igual a string gabarito do level atual. Foram criadas funções auxiliares para checar qual tabuleiro gabarito usar dado que seria feita uma melhoria para usar vector, porém não foi possível fazer a tempo. 

Quando um level é passado um novo botão aparece indicando que o próximo level está livre. Isso chama a função de próximo level. 

Alguns problemas foram encontrados com o funcionamento dos botões e coloração o que impossibilitou em fazer melhorias no código, como mudança de array para vector para evitar criar um vetor para cada level, na função lambda há muita repetição de código entre outras melhorias.