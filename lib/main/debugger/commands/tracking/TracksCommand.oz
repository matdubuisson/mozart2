local
  proc {DisplayOptions}
    {DisplayFrame "Tracks command without options" [
      "<condition>" "no arguments taken only an optional filtering condition"
    ]}
  end

  proc {DisplayTracks Arguments}
    Conditions = {ExtractConditions Arguments $}
  in
    if Conditions \= none then
      Tracks = {Boot_EventManager.getTracks $}
    in
      {DisplayJournalCSV
        [
          "Type            " "Category        " "Index   "
          "Author Id" flexible("Victim Ids                      ")
        ]
        Tracks
        proc {$ Type Category Track ?Result}
          case Track of track(
            index: Index
            authorId: AuthorId
            victimIds: VictimIds
          ) then
            Result = track(
              type: Type
              category: Category
              index: Index
              authorId: AuthorId
              victimIds: VictimIds
            )
          end
        end
        proc {$ Track ?Result}
          Result = {CheckConditions Conditions Track $}
        end
        proc {$ Track ?Result}
          case Track of track(
            type: Type
            category: Category
            index: Index
            authorId: AuthorId
            victimIds: VictimIds
          ) then
            Result = [
              {Atom.toString Type $}
              {Atom.toString Category $}
              {Int.toString Index $}
              case AuthorId of none then "none"
              else {Int.toString AuthorId $} end
              {ListToString VictimIds $}
            ]
          end
        end}
    end
  end
in
  case Arguments of nil then
    {DisplayTracks nil}
  [] "help"|_ then
    {DisplayOptions}
  else
    {DisplayTracks Arguments}
  end
end
