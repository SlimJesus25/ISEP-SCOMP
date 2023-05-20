# Question #

The father only writes the value on the screen at the end. Review the results. Will these results always be correct? Propose a solution that ensures data consistency.

# Answer #

Os resultados obtidos nem sempre vão estar corretos uma vez que ambos os processos vão estar a acessar e a modificar valores de variáveis na memória partilhada ao mesmo tempo.


Para garantir a consistência dos dados é necessário implementar um mecanismo de sincronização do acesso à memória partilhada, por exemplo: a utilização de semáforos ou active waiting.